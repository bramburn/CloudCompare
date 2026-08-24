// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.     #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// ##########################################################################

/**
 * @file ReCapFilter.cpp
 *
 * @brief Autodesk ReCap RCS/RCP filter implementation
 *
 * Imports Autodesk ReCap scan data:
 * - **RCS**: ReCap Scan — individual scan station
 * - **RCP**: ReCap Project — multi-station project
 *
 * ## Format
 *
 * ReCap stores TLS (terrestrial laser scanner) data:
 * - Registered scan positions (trajectory)
 * - Point cloud with color from registered photography
 * - Spherical panoramas per station
 * - Scan metadata (scanner model, date, weather)
 *
 * ## Import
 *
 * Uses the Autodesk ReCap SDK (RCStructuredScan, RCSphericalModel).
 * Each scan station becomes a separate point cloud entity.
 *
 * @see ReCapFilter.h
 */
#include "ReCapFilter.h"

// ReCap SDK
#include <data/IRCPointIterator.h>
#include <data/RCPointIteratorSettings.h>
#include <data/RCProject.h>
#include <data/RCProjectImporter.h>
#include <data/RCScan.h>
#include <foundation/RCBox.h>
#include <foundation/RCCode.h>
#include <foundation/RCSharedPtr.h>
#include <foundation/RCTransform.h>
#include <foundation/RCVector.h>

// qCC_db
#include <ccColorScalesManager.h>
#include <ccGlobalShiftManager.h>
#include <ccLog.h>
#include <ccNormalVectors.h>
#include <ccPointCloud.h>
#include <ccProgressDialog.h>
#include <ccScalarField.h>

// ReCap SDK attributes
#include <data/RCProjectLoadAttributes.h>

// CCCoreLib
#include <GenericProgressCallback.h>

// Qt
#include <QApplication>
#include <QFileInfo>
#include <QString>

// System
#include <cassert>
#include <vector>

using namespace Autodesk::RealityComputing;
using namespace Autodesk::RealityComputing::Data;
using Foundation::RCBox;
using Foundation::RCCode;
using Foundation::RCSharedPtr;
using Foundation::RCTransform;
using Foundation::RCVector3d;

namespace
{
	// -------------------------------------------------------------------------
	// Convert ReCap RGB to CloudCompare colour and store in the RGB table.
	// -------------------------------------------------------------------------
	void SetPointColor(ccPointCloud* cloud, const Foundation::RCVector4ub& rgba)
	{
		cloud->addColor(static_cast<ColorCompType>(rgba.x),
		                static_cast<ColorCompType>(rgba.y),
		                static_cast<ColorCompType>(rgba.z));
	}

	// -------------------------------------------------------------------------
	// Build a ccPointCloud from an RCScan, iterating all points.
	// Returns nullptr on failure.
	// -------------------------------------------------------------------------
	ccPointCloud* LoadRCScan(RCScan& scan, ccProgressDialog* progressDlg)
	{
		const QString scanName = QString::fromUtf16(
		    reinterpret_cast<const ushort*>(scan.getName().getWString()));

		uint64_t totalPoints = scan.getNumberOfPoints();
		if (totalPoints == 0)
		{
			ccLog::Warning("[ReCap] Scan '%1' contains no points.", scanName);
			return nullptr;
		}

		// Configure iterator for maximum density.
		RCPointIteratorSettings settings;
		settings.setIsReadOnly(true);
		settings.setDensity(0.0); // 0 = maximum density

		bool hasColors = scan.hasColors();
		bool hasIntensities = scan.hasIntensities();
		bool hasNormals = scan.hasNormals();

		// Create iterator.
		RCSharedPtr<IRCPointIterator> iter = scan.createPointIterator(settings);
		if (!iter)
		{
			ccLog::Error("[ReCap] Failed to create point iterator for '%1'.",
			             scanName);
			return nullptr;
		}

		// Allocate cloud.
		ccPointCloud* cloud = new ccPointCloud(scanName);
		if (!cloud->reserve(static_cast<unsigned>(totalPoints)))
		{
			delete cloud;
			ccLog::Error("[ReCap] Out of memory loading scan '%1' (%2 points).",
			             scanName,
			             totalPoints);
			return nullptr;
		}

		// Allocate per-point RGB colour buffer.
		if (hasColors)
		{
			if (!cloud->reserveTheRGBTable())
			{
				hasColors = false;
				ccLog::Warning("[ReCap] Could not allocate RGB table for scan '%1'.",
				               scanName);
			}
		}

		// Allocate intensity scalar field.
		ccScalarField* intensitySF = nullptr;
		if (hasIntensities)
		{
			intensitySF = new ccScalarField(std::string("Intensity"));
			int sfIdx = cloud->addScalarField(intensitySF);
			if (sfIdx < 0)
			{
				// addScalarField failed — cloud did not take ownership; release our ref.
				intensitySF->release();
				intensitySF = nullptr;
				hasIntensities = false;
			}
		}

		// Allocate normals table.
		if (hasNormals)
		{
			if (!cloud->reserveTheNormsTable())
			{
				hasNormals = false;
				ccLog::Warning("[ReCap] Could not allocate normals table for scan '%1'.",
				               scanName);
			}
		}

		// Progress tracking.
		// Set info on the dialog (ccProgressDialog) directly; NormalizedProgress
		// wraps it and just calls update()/isCancelRequested().
		if (progressDlg)
		{
			progressDlg->setMethodTitle(QObject::tr("Loading ReCap scan '%1'").arg(scanName));
			progressDlg->setInfo(QObject::tr("%1 points").arg(totalPoints));
		}
		CCCoreLib::NormalizedProgress nprogress(progressDlg,
		                                        static_cast<unsigned>(totalPoints));

		// Iterate and populate the cloud.
		iter->reset();
		while (!iter->atEnd())
		{
			IRCPointAccessor& pt = iter->getPoint();

			// Position — CCVector3 is Vector3Tpl<PointCoordinateType> (float).
			const RCVector3d& rcPos = pt.getPosition();
			cloud->addPoint(CCVector3(static_cast<PointCoordinateType>(rcPos.x),
			                          static_cast<PointCoordinateType>(rcPos.y),
			                          static_cast<PointCoordinateType>(rcPos.z)));

			// Colour — stored in the RGB table.
			if (hasColors)
			{
				SetPointColor(cloud, pt.getColor());
			}

			// Intensity — append to the scalar field.
			if (hasIntensities && intensitySF != nullptr)
			{
				// getIntensity() returns the raw float; cast to ScalarType (double).
				intensitySF->addElement(static_cast<ScalarType>(pt.getIntensity()));
			}

			// Normal — compressed and stored in the normals table.
			if (hasNormals)
			{
				RCVector3d rcNorm = pt.getNormal();
				PointCoordinateType normalArr[3] = {
				    static_cast<PointCoordinateType>(rcNorm.x),
				    static_cast<PointCoordinateType>(rcNorm.y),
				    static_cast<PointCoordinateType>(rcNorm.z)};
				cloud->addNormIndex(ccNormalVectors::GetNormIndex(normalArr));
			}

			iter->moveToNextPoint();

			if (progressDlg && !nprogress.oneStep())
			{
				iter->close();
				delete cloud;
				return nullptr;
			}
		}

		iter->close();

		// Finalise colour display.
		if (hasColors)
		{
			cloud->showColors(true);
		}

		// Finalise intensity field.
		if (hasIntensities && intensitySF != nullptr)
		{
			// intensitySF is still valid — addScalarField takes ownership but the cloud
			// keeps the underlying ScalarField alive via refcounting.
			intensitySF->computeMinAndMax();
			intensitySF->setSaturationStart(intensitySF->getMin());
			intensitySF->setSaturationStop(intensitySF->getMax());
			intensitySF->setColorScale(
			    ccColorScalesManager::GetDefaultScale(ccColorScalesManager::GREY));
			cloud->setCurrentScalarField(
			    cloud->getScalarFieldIndexByName(intensitySF->getName().c_str()));
			cloud->showSF(true);
		}

		// Finalise normals.
		if (hasNormals)
		{
			cloud->showNormals(true);
		}

		// Apply the scan's stored transform if present.
		RCTransform rcTransform;
		if (scan.getFullTransform(rcTransform))
		{
			// Extract translation.
			const RCVector3d& t = rcTransform.getTranslation();

			// Extract 3x3 rotation column-by-column from the rotation matrix.
			const Foundation::RCRotationMatrix& rot = rcTransform.getRotation();
			CCVector3 col0(static_cast<PointCoordinateType>(rot.getColumn(0).x),
			               static_cast<PointCoordinateType>(rot.getColumn(1).x),
			               static_cast<PointCoordinateType>(rot.getColumn(2).x));
			CCVector3 col1(static_cast<PointCoordinateType>(rot.getColumn(0).y),
			               static_cast<PointCoordinateType>(rot.getColumn(1).y),
			               static_cast<PointCoordinateType>(rot.getColumn(2).y));
			CCVector3 col2(static_cast<PointCoordinateType>(rot.getColumn(0).z),
			               static_cast<PointCoordinateType>(rot.getColumn(1).z),
			               static_cast<PointCoordinateType>(rot.getColumn(2).z));
			CCVector3 col3(static_cast<PointCoordinateType>(t.x),
			               static_cast<PointCoordinateType>(t.y),
			               static_cast<PointCoordinateType>(t.z));

			ccGLMatrix ccMat(col0, col1, col2, col3);
			cloud->setGLTransformation(ccMat);
		}

		ccLog::Print("[ReCap] Loaded scan '%1': %2 points, %3 colours, "
		             "%4 intensities, %5 normals",
		             scanName,
		             cloud->size(),
		             hasColors ? "has" : "no",
		             hasIntensities ? "has" : "no",
		             hasNormals ? "has" : "no");

		return cloud;
	}

} // namespace

// =============================================================================
// FileIOFilter overrides
// =============================================================================

ReCapFilter::ReCapFilter()
    : FileIOFilter({"_ReCap I/O",              // unique ID (underscore prefix avoids clash)
                    50.0f,                     // priority — higher than generic OBJ/PLY
                    QStringList{"rcs", "rcp"}, // extensions (lowercase for CC)
                    "rcs",                     // default extension
                    QStringList{"ReCap scan (*.rcs)", "ReCap project (*.rcp)"},
                    QStringList{}, // no save filters (read-only)
                    Import})       // import only
{
	ccLog::Warning("[ReCap] ReCapFilter constructed — extensions: rcs rcp");
}

CC_FILE_ERROR ReCapFilter::loadFile(const QString& filename,
                                    ccHObject& container,
                                    LoadParameters& parameters)
{
	QFileInfo fileInfo(filename);
	QString ext = fileInfo.suffix().toLower();

	ccLog::Warning(QStringLiteral("[ReCap] loadFile called: %1 (ext=%2)").arg(filename, ext));

	QScopedPointer<ccProgressDialog> pDlg(nullptr);
	if (parameters.parentWidget)
	{
		pDlg.reset(new ccProgressDialog(true, parameters.parentWidget));
		pDlg->setMethodTitle(QObject::tr("Loading ReCap file"));
	}

	RCCode rcCode;

	if (ext == "rcs")
	{
		// ---- RCS: single stationary or mobile scan file ----
		Foundation::RCString rcPath(reinterpret_cast<const wchar_t*>(filename.utf16()));

		Foundation::RCSharedPtr<RCScan> scan =
		    RCScan::loadFile(rcPath,
		                     RCFileAccess::ReadOnly,
		                     rcCode);

		if (!scan)
		{
			ccLog::Error("[ReCap] Failed to load RCS file '%1'  (ReCap error %2).",
			             filename,
			             static_cast<int>(rcCode));
			return CC_FERR_BAD_ARGUMENT;
		}

		ccPointCloud* cloud = LoadRCScan(*scan, pDlg.data());
		if (!cloud)
			return CC_FERR_NO_LOAD;

		container.addChild(cloud, ccHObject::DP_PARENT_OF_OTHER);
	}
	else if (ext == "rcp")
	{
		// ---- RCP: ReCap Photo project — contains multiple scans ----
		Foundation::RCString rcPath(reinterpret_cast<const wchar_t*>(filename.utf16()));

		// RCP is a project container — use RCProject::loadFromFile.
		Foundation::RCSharedPtr<RCProject> project =
		    RCProject::loadFromFile(rcPath,
		                            RCFileAccess::ReadOnly,
		                            RCProjectUserEdits::None,
		                            rcCode);

		if (!project)
		{
			ccLog::Error("[ReCap] Failed to load RCP project '%1'  (ReCap error %2).",
			             filename,
			             static_cast<int>(rcCode));
			return CC_FERR_BAD_ARGUMENT;
		}

		int scanCount = project->getNumberOfScans();
		if (scanCount == 0)
		{
			ccLog::Warning("[ReCap] RCP project '%1' contains no scans.", filename);
			return CC_FERR_NO_LOAD;
		}

		if (pDlg)
		{
			pDlg->setInfo(QObject::tr("%1 scans").arg(scanCount));
			pDlg->start();
			QApplication::processEvents();
		}

		for (int i = 0; i < scanCount; ++i)
		{
			Foundation::RCSharedPtr<RCScan> scan_i = project->getScanAt(i);
			if (!scan_i)
				continue;

			ccPointCloud* cloud = LoadRCScan(*scan_i, pDlg.data());
			if (cloud)
			{
				container.addChild(cloud, ccHObject::DP_PARENT_OF_OTHER);
			}

			if (pDlg && pDlg->isCancelRequested())
				break;
		}
	}
	else
	{
		return CC_FERR_WRONG_FILE_TYPE;
	}

	ccLog::Warning("[ReCap] Large-coordinate global shift is handled by CloudCompare "
	               "automatically on load (see Edit > Global Shift & Scale).");

	return CC_FERR_NO_ERROR;
}

bool ReCapFilter::canSave(CC_CLASS_ENUM type,
                          bool& multiple,
                          bool& exclusive) const
{
	Q_UNUSED(type);
	Q_UNUSED(multiple);
	Q_UNUSED(exclusive);
	return false;
}

CC_FILE_ERROR ReCapFilter::saveToFile(ccHObject* entity,
                                      const QString& filename,
                                      const SaveParameters& parameters)
{
	Q_UNUSED(entity);
	Q_UNUSED(filename);
	Q_UNUSED(parameters);
	return CC_FERR_NOT_IMPLEMENTED;
}
