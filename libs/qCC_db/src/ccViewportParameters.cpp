// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccViewportParameters.cpp
 *
 * @brief Viewport parameters implementation
 *
 * Implements camera math for CloudCompare's 3D viewport:
 * view matrix construction, focal distance management, pixel size
 * computation, and serialization with backward compatibility.
 *
 * @see ccViewportParameters
 */

#include "ccViewportParameters.h"

// CCCoreLib
#include <CCConst.h>

// Qt
#include <QRect>

// ccViewportParameters::ccViewportParameters
/**
 * @brief Default constructor
 *
 * Initializes to a safe default state:
 * - Identity view rotation
 * - Orthographic projection (perspectiveView = false)
 * - Object-centered rotation (objectCenteredView = true)
 * - Focal distance = 1.0 (camera at z=1.0, pivot at origin)
 * - 50° FOV, 1:1 aspect ratio
 * - No manual clipping (NaN depths)
 */
ccViewportParameters::ccViewportParameters()
    : defaultPointSize(1)
    , defaultLineWidth(1)
    , perspectiveView(false)
    , objectCenteredView(true)
    , zNearCoef(0.005)
    , nearClippingDepth(std::numeric_limits<double>::quiet_NaN())
    , farClippingDepth(std::numeric_limits<double>::quiet_NaN())
    , zNear(0)
    , zFar(0)
    , fov_deg(50.0f)
    , cameraAspectRatio(1.0f)
    , focalDistance(1.0)
    , pivotPoint(0, 0, 0)
    , cameraCenter(0, 0, focalDistance)
{
	viewMat.toIdentity();
}

// ccViewportParameters::ccViewportParameters (copy)
ccViewportParameters::ccViewportParameters(const ccViewportParameters& params)
    : viewMat(params.viewMat)
    , defaultPointSize(params.defaultPointSize)
    , defaultLineWidth(params.defaultLineWidth)
    , perspectiveView(params.perspectiveView)
    , objectCenteredView(params.objectCenteredView)
    , zNearCoef(params.zNearCoef)
    , nearClippingDepth(params.nearClippingDepth)
    , farClippingDepth(params.farClippingDepth)
    , zNear(params.zNear)
    , zFar(params.zFar)
    , fov_deg(params.fov_deg)
    , cameraAspectRatio(params.cameraAspectRatio)
    , focalDistance(params.focalDistance)
    , pivotPoint(params.pivotPoint)
    , cameraCenter(params.cameraCenter)
{
}

// ccViewportParameters::toFile
/**
 * @brief Serialize parameters to binary file
 *
 * Format (version >= 53):
 * - viewMat (16 doubles)
 * - focalDistance, defaultPointSize, defaultLineWidth
 * - perspectiveView, objectCenteredView (bools)
 * - pivotPoint (3 doubles), cameraCenter (3 doubles)
 * - fov_deg, cameraAspectRatio
 * - nearClippingDepth, farClippingDepth
 *
 * @param[in] out        Output file (must be open for writing)
 * @param[in] dataVersion File format version
 * @return true on success
 */
bool ccViewportParameters::toFile(QFile& out, short dataVersion) const
{
	assert(out.isOpen() && (out.openMode() & QIODevice::WriteOnly));
	if (dataVersion < 51)
	{
		assert(false);
		return false;
	}

	// Base modelview matrix (dataVersion >= 20)
	if (!viewMat.toFile(out, dataVersion))
		return false;

	// Other scalar parameters
	QDataStream outStream(&out);
	outStream << focalDistance;
	outStream << defaultPointSize;
	outStream << defaultLineWidth;
	outStream << perspectiveView;
	outStream << objectCenteredView;
	outStream << pivotPoint.x;
	outStream << pivotPoint.y;
	outStream << pivotPoint.z;
	outStream << cameraCenter.x;
	outStream << cameraCenter.y;
	outStream << cameraCenter.z;
	outStream << fov_deg;
	outStream << cameraAspectRatio;

	// Clipping depths (dataVersion >= 53)
	if (dataVersion >= 53)
	{
		outStream << nearClippingDepth;
		outStream << farClippingDepth;
	}

	return true;
}

// ccViewportParameters::fromFile
/**
 * @brief Deserialize parameters from binary file
 *
 * Handles backward compatibility with older file versions:
 * - dataVersion < 36: viewMat stored as float (converted to double)
 * - dataVersion < 51: pixelSize + zoom used instead of focalDistance
 * - dataVersion < 53: no clipping depth storage
 *
 * @param[in]  in              Input file
 * @param[in]  dataVersion     File format version
 * @param[in]  flags           Deserialization flags
 * @param[out] oldToNewIDMap  ID remapping table
 * @return true on success
 */
bool ccViewportParameters::fromFile(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap)
{
	// Load view rotation matrix
	if (dataVersion >= 36)
	{
		// Stored in double precision since version 36
		if (!viewMat.fromFile(in, dataVersion, flags, oldToNewIDMap))
			return false;
	}
	else
	{
		// Stored as float before version 36 — convert to double
		ccGLMatrix _viewMat;
		if (!_viewMat.fromFile(in, dataVersion, flags, oldToNewIDMap))
			return false;
		viewMat = ccGLMatrixd(_viewMat.data());
	}

	QDataStream inStream(&in);
	float zoom = 1.0f;
	float pixelSize = 0.0f;

	// focalDistance replaced pixelSize+zoom in version 51
	if (dataVersion < 51)
	{
		inStream >> pixelSize;
		inStream >> zoom;
	}
	else
	{
		inStream >> focalDistance;
	}

	inStream >> defaultPointSize;
	inStream >> defaultLineWidth;
	inStream >> perspectiveView;
	inStream >> objectCenteredView;

	// pivotPoint and cameraCenter: double precision since version 36
	if (dataVersion >= 36)
	{
		inStream >> pivotPoint.x;
		inStream >> pivotPoint.y;
		inStream >> pivotPoint.z;
		inStream >> cameraCenter.x;
		inStream >> cameraCenter.y;
		inStream >> cameraCenter.z;
	}
	else
	{
		CCVector3 _pivotPoint;
		ccSerializationHelper::CoordsFromDataStream(inStream, flags, _pivotPoint.u, 3);
		pivotPoint = _pivotPoint;

		if (dataVersion >= 25)
		{
			// cameraCenter stored separately since version 25
			CCVector3 _cameraCenter;
			ccSerializationHelper::CoordsFromDataStream(inStream, flags, _cameraCenter.u, 3);
			cameraCenter = _cameraCenter;
		}
		else
		{
			// Before version 25: cameraCenter = pivotPoint (incorrect for perspective)
			cameraCenter = pivotPoint;
		}
	}

	inStream >> fov_deg;
	inStream >> cameraAspectRatio;

	// screenPan was replaced by cameraCenter in version 25
	if (dataVersion < 25)
	{
		float screenPan[2];
		inStream >> screenPan[0];
		inStream >> screenPan[1];

		if (objectCenteredView)
		{
			cameraCenter.x += static_cast<double>(screenPan[0]);
			cameraCenter.y += static_cast<double>(screenPan[1]);
		}
	}

	// Ortho aspect ratio was stored separately before version 51
	if (dataVersion >= 30 && dataVersion < 51)
	{
		float orthoAspectRatio = 0.0f;
		inStream >> orthoAspectRatio;
	}

	// Reconstruct focalDistance from pixelSize+zoom for pre-51 files
	if (dataVersion < 51 && zoom != 1.0f)
	{
		if (perspectiveView)
		{
			focalDistance = (cameraCenter - pivotPoint).normd();
		}
		else
		{
			static int DefaultScreenSize_pix = 2048; // Average screen size
			focalDistance = pixelSize * static_cast<double>(DefaultScreenSize_pix)
			                / computeDistanceToWidthRatio(DefaultScreenSize_pix, DefaultScreenSize_pix);
		}
		setFocalDistance(focalDistance / zoom);
		ccLog::Warning("[ccViewportParameters] Approximate focal distance (sorry, the parameters of viewport objects have changed!)");
	}

	// Clipping depths: NaN before version 53
	if (dataVersion < 53)
	{
		nearClippingDepth = farClippingDepth = std::numeric_limits<double>::quiet_NaN();
	}
	else
	{
		inStream >> nearClippingDepth;
		inStream >> farClippingDepth;
	}

	return true;
}

// ccViewportParameters::minimumFileVersion
short ccViewportParameters::minimumFileVersion() const
{
	// Version 53 needed for non-NaN clipping depths
	short minVersion = (std::isnan(nearClippingDepth) && std::isnan(farClippingDepth) ? 51 : 53);
	return std::max(minVersion, viewMat.minimumFileVersion());
}

// ccViewportParameters::getRotationCenter
const CCVector3d& ccViewportParameters::getRotationCenter() const
{
	return (objectCenteredView ? pivotPoint : cameraCenter);
}

// ccViewportParameters::computeViewMatrix
/**
 * @brief Compute the OpenGL view matrix
 *
 * Constructs a gluLookAt-style view matrix by:
 * 1. Translating world origin to the rotation center (T(-C))
 * 2. Applying the rotation from viewMat (R)
 * 3. Translating back to camera position (T(C) but offset by cameraCenter - rotationCenter)
 *
 * Result: the scene appears rotated around the pivot (object-centered)
 *         or around the camera eye (viewer-centered).
 */
ccGLMatrixd ccViewportParameters::computeViewMatrix() const
{
	ccGLMatrixd viewMatd;
	viewMatd.toIdentity();

	const CCVector3d& rotationCenter = getRotationCenter();

	// Step 1: translate to rotation center
	viewMatd.setTranslation(-rotationCenter);

	// Step 2: apply rotation
	viewMatd = viewMat * viewMatd;

	// Step 3: translate to camera position
	viewMatd.setTranslation(viewMatd.getTranslationAsVec3D() + rotationCenter - cameraCenter);

	return viewMatd;
}

// ccViewportParameters::computeScaleMatrix
/**
 * @brief Compute the aspect-ratio correction scale matrix
 *
 * When the viewport is taller than wide (adjusted ar < 1.0), applies
 * a uniform scale of (ar, ar, 1.0) to prevent distortion. This
 * compensates for the non-square GL viewport by scaling down in X/Y.
 */
ccGLMatrixd ccViewportParameters::computeScaleMatrix(const QRect& glViewport) const
{
	ccGLMatrixd scaleMatd;
	scaleMatd.toIdentity();

	if (glViewport.height() != 0)
	{
		double ar = static_cast<double>(glViewport.width() / (glViewport.height() * cameraAspectRatio));
		if (ar < 1.0)
		{
			// Apply (ar, ar, 1.0) scale to correct for tall viewport
			scaleMatd.data()[0] = ar;
			scaleMatd.data()[5] = ar;
		}
	}

	return scaleMatd;
}

// ccViewportParameters::getViewDir
/**
 * @brief Get the view direction (camera forward vector)
 *
 * Extracts the 3rd row of viewMat (the column of the matrix that
 * represents the -Z axis in view space, transformed to world space)
 * and normalizes it. Points from the camera into the scene.
 */
CCVector3d ccViewportParameters::getViewDir() const
{
	const double* M = viewMat.data();
	CCVector3d axis(-M[2], -M[6], -M[10]);
	axis.normalize();
	return axis;
}

// ccViewportParameters::getUpDir
/**
 * @brief Get the screen-up direction
 *
 * Extracts the 2nd row of viewMat and normalizes it. Represents
 * the direction that corresponds to "up" on the screen.
 */
CCVector3d ccViewportParameters::getUpDir() const
{
	const double* M = viewMat.data();
	CCVector3d axis(M[1], M[5], M[9]);
	axis.normalize();
	return axis;
}

// ccViewportParameters::setPivotPoint
void ccViewportParameters::setPivotPoint(const CCVector3d& P, bool autoUpdateFocal)
{
	pivotPoint = P;
	if (autoUpdateFocal && objectCenteredView)
	{
		// Maintain visual zoom by keeping camera at the same distance
		focalDistance = cameraCenter.z - pivotPoint.z;
	}
}

// ccViewportParameters::setCameraCenter
void ccViewportParameters::setCameraCenter(const CCVector3d& C, bool autoUpdateFocal)
{
	cameraCenter = C;
	if (autoUpdateFocal && objectCenteredView)
	{
		// Maintain visual zoom by keeping pivot at the same distance
		focalDistance = cameraCenter.z - pivotPoint.z;
	}
}

// ccViewportParameters::setFocalDistance
void ccViewportParameters::setFocalDistance(double distance)
{
	focalDistance = distance;
	if (objectCenteredView)
	{
		// Keep pivot fixed; move camera to maintain focal distance
		cameraCenter.z = pivotPoint.z + focalDistance;
	}
}

// ccViewportParameters::computeDistanceToHalfWidthRatio
/**
 * @brief Compute tan(fov/2)
 *
 * Half of the tangent of the half-field-of-view. This ratio converts
 * a depth value into a half-width, and vice versa:
 * halfWidth = ratio * depth
 */
double ccViewportParameters::computeDistanceToHalfWidthRatio() const
{
	return std::tan(CCCoreLib::DegreesToRadians(fov_deg / 2.0));
}

// ccViewportParameters::computeDistanceToWidthRatio
/**
 * @brief Compute the full width-to-distance ratio
 *
 * Returns 2 * tan(fov/2) / min(ar, 1.0), where ar is the adjusted
 * aspect ratio accounting for cameraAspectRatio and screen shape.
 * When the screen is wider than tall, the ratio is unchanged.
 * When the screen is taller than wide, the ratio increases.
 */
double ccViewportParameters::computeDistanceToWidthRatio(int screenWidth, int screenHeight) const
{
	if (screenHeight <= 0 || screenWidth <= 0)
	{
		assert(false);
		return 1.0;
	}

	// Adjust for camera aspect ratio, then clamp to ≤ 1.0
	double ar = std::min(1.0, static_cast<double>(screenWidth / (screenHeight * cameraAspectRatio)));
	return (2.0 * computeDistanceToHalfWidthRatio()) / ar;
}

// ccViewportParameters::computeWidthAtFocalDist
double ccViewportParameters::computeWidthAtFocalDist(int screenWidth, int screenHeight) const
{
	return getFocalDistance() * computeDistanceToWidthRatio(screenWidth, screenHeight);
}

// ccViewportParameters::computePixelSize
double ccViewportParameters::computePixelSize(int screenWidth, int screenHeight) const
{
	return (screenWidth > 0 ? computeWidthAtFocalDist(screenWidth, screenHeight) / screenWidth : 1.0);
}

// ccViewportParameters::log
void ccViewportParameters::log() const
{
	ccLog::Print("View Matrix");
	ccLog::Print(viewMat.toString());
	ccLog::Print(QString("Default point size: %1").arg(defaultPointSize));
	ccLog::Print(QString("Default line width: %1").arg(defaultLineWidth));
	ccLog::Print(QString("Perspective view: %1").arg(perspectiveView ? "yes" : "no"));
	ccLog::Print(QString("Object-centered view: %1").arg(objectCenteredView ? "yes" : "no"));
	ccLog::Print(QString("zNearCoef: %1").arg(zNearCoef));
	ccLog::Print(QString("nearClippingDepth: %1").arg(nearClippingDepth));
	ccLog::Print(QString("farClippingDepth: %1").arg(farClippingDepth));
	ccLog::Print(QString("zNear: %1").arg(zNear));
	ccLog::Print(QString("zFar: %1").arg(zFar));
	ccLog::Print(QString("fov: %1 deg").arg(fov_deg));
	ccLog::Print(QString("camera a.r.: %1").arg(cameraAspectRatio));
	ccLog::Print(QString("focal distance: %1").arg(getFocalDistance()));
	ccLog::Print(QString("pivot point:(%1 ; %2; %3)").arg(pivotPoint.x).arg(pivotPoint.y).arg(pivotPoint.z));
	ccLog::Print(QString("camera center:(%1 ; %2; %3)").arg(cameraCenter.x).arg(cameraCenter.y).arg(cameraCenter.z));
}
