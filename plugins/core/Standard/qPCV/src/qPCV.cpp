//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qPCV                        #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

/**
 * @file qPCV.cpp
 *
 * @brief Potentially Visible Set (PCV) plugin implementation
 *
 * Implements the ShadeVis ambient occlusion computation pipeline.
 *
 * Ray generation:
 * - Mode 1: use normals of another cloud as ray directions
 * - Mode 2: generate uniform spherical sampling via PCV::GenerateRays
 *
 * Output: per-point scalar field [0,1] where 0=occluded, 1=visible.
 *
 * @see qPCV
 */

#include "qPCV.h"
#include "ccPcvDlg.h"
#include "PCVCommand.h"

// CCCoreLib
#include <PCV.h>
#include <ScalarField.h>

// qCC_db
#include <ccGenericMesh.h>
#include <ccGenericPointCloud.h>
#include <ccHObjectCaster.h>
#include <ccPointCloud.h>
#include <ccProgressDialog.h>
#include <ccScalarField.h>

// Qt
#include <QElapsedTimer>
#include <QMainWindow>
#include <QProgressBar>

// Semi-persistent dialog settings (reset per session)
static bool s_firstLaunch           = true;
static int  s_raysSpinBoxValue     = 256;
static int  s_resSpinBoxValue      = 1024;
static bool s_mode180CheckBoxState = true;
static bool s_closedMeshCheckBoxState = false;

// qPCV::qPCV
/**
 * @brief Construct the PCV plugin
 *
 * Initializes ccStdPluginInterface with embedded metadata
 * (":/CC/plugin/qPCV/info.json").
 */
qPCV::qPCV(QObject* parent /*=nullptr*/)
    : QObject(parent)
    , ccStdPluginInterface(":/CC/plugin/qPCV/info.json")
    , m_action(nullptr)
{
}

// qPCV::onNewSelection
/**
 * @brief Handle selection change
 *
 * Enables the action when at least one point cloud or mesh is selected.
 *
 * @param[in] selectedEntities Current selection
 */
void qPCV::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action)
	{
		bool eligible = false;
		for (ccHObject* obj : selectedEntities)
		{
			if (obj && (obj->isKindOf(CC_TYPES::POINT_CLOUD) || obj->isKindOf(CC_TYPES::MESH)))
			{
				eligible = true;
				break;
			}
		}
		m_action->setEnabled(eligible);
	}
}

// qPCV::getActions
/**
 * @brief Create and return the plugin action
 *
 * Lazily creates the "PCV / ShadeVis" action.
 *
 * @return List containing the PCV action
 */
QList<QAction*> qPCV::getActions()
{
	if (!m_action)
	{
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());
		connect(m_action, &QAction::triggered, this, &qPCV::doAction);
	}
	return QList<QAction*>{m_action};
}

// qPCV::doAction
/**
 * @brief Execute the PCV / ShadeVis ambient occlusion computation
 *
 * Pipeline:
 * 1. Collect all selected point clouds and meshes
 * 2. Scan the full DB tree for clouds-with-normals (ray source option)
 * 3. Show parameter dialog with semi-persistent state
 * 4. Generate ray set: from selected cloud's normals OR PCV::GenerateRays
 * 5. Call PCVCommand::Process() on all candidates
 * 6. Log timing and refresh all views
 */
void qPCV::doAction()
{
	assert(m_app);
	if (!m_app)
		return;

	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();

	// Collect candidates (clouds and meshes)
	ccHObject::Container candidates;
	bool hasMeshes = false;
	for (ccHObject* obj : selectedEntities)
	{
		if (!obj)
		{
			assert(false);
			continue;
		}

		if (obj->isA(CC_TYPES::POINT_CLOUD))
		{
			candidates.push_back(obj);
		}
		else if (obj->isKindOf(CC_TYPES::MESH))
		{
			ccGenericMesh* mesh = ccHObjectCaster::ToGenericMesh(obj);
			if (mesh->getAssociatedCloud() && mesh->getAssociatedCloud()->isA(CC_TYPES::POINT_CLOUD))
			{
				candidates.push_back(obj);
				hasMeshes = true;
			}
		}
	}

	// Show dialog
	ccPcvDlg dlg(m_app->getMainWindow());

	// Restore semi-persistent state
	if (!s_firstLaunch)
	{
		dlg.raysSpinBox->setValue(s_raysSpinBoxValue);
		dlg.mode180CheckBox->setChecked(s_mode180CheckBoxState);
		dlg.resSpinBox->setValue(s_resSpinBoxValue);
		dlg.closedMeshCheckBox->setChecked(s_closedMeshCheckBoxState);
	}
	dlg.closedMeshCheckBox->setEnabled(hasMeshes);

	// Scan DB tree for clouds-with-normals (ray source)
	std::vector<ccGenericPointCloud*> cloudsWithNormals;
	ccHObject* root = m_app->dbRootObject();
	if (root)
	{
		ccHObject::Container clouds;
		root->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

		for (auto& pointCloud : clouds)
		{
			ccGenericPointCloud* cloud = ccHObjectCaster::ToGenericPointCloud(pointCloud);
			if (cloud && cloud->hasNormals())
			{
				cloudsWithNormals.push_back(cloud);
				QString cloudTitle = QStringLiteral("%1 - %2 points").arg(cloud->getName()).arg(cloud->size());
				if (cloud->getParent() && cloud->getParent()->isKindOf(CC_TYPES::MESH))
					cloudTitle.append(QStringLiteral(" (%1)").arg(cloud->getParent()->getName()));
				dlg.cloudsComboBox->addItem(cloudTitle);
			}
		}
	}

	if (cloudsWithNormals.empty())
		dlg.useCloudRadioButton->setEnabled(false);

	if (!dlg.exec())
		return;

	// Save dialog state for next invocation
	s_firstLaunch            = false;
	s_raysSpinBoxValue       = dlg.raysSpinBox->value();
	s_mode180CheckBoxState   = dlg.mode180CheckBox->isChecked();
	s_resSpinBoxValue        = dlg.resSpinBox->value();
	s_closedMeshCheckBoxState = dlg.closedMeshCheckBox->isChecked();

	unsigned rayCount   = dlg.raysSpinBox->value();
	unsigned resolution = dlg.resSpinBox->value();
	bool meshIsClosed  = hasMeshes ? dlg.closedMeshCheckBox->isChecked() : false;
	bool mode360       = !dlg.mode180CheckBox->isChecked();

	// Generate ray set
	std::vector<CCVector3d> rays;
	if (!cloudsWithNormals.empty() && dlg.useCloudRadioButton->isChecked())
	{
		// Use another cloud's normals as ray directions
		assert(dlg.cloudsComboBox->currentIndex() < static_cast<int>(cloudsWithNormals.size()));
		ccGenericPointCloud* pc = cloudsWithNormals[dlg.cloudsComboBox->currentIndex()];
		try
		{
			rays.resize(pc->size());
		}
		catch (std::bad_alloc)
		{
			m_app->dispToConsole("Not enough memory to generate the set of rays", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			return;
		}
		for (unsigned i = 0; i < pc->size(); ++i)
			rays[i] = CCVector3d(pc->getPointNormal(i));
	}
	else
	{
		// Generate uniform spherical sampling
		if (!PCV::GenerateRays(rayCount, rays, mode360))
		{
			m_app->dispToConsole("Failed to generate the set of rays", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			return;
		}
	}

	if (rays.empty())
	{
		assert(false);
		m_app->dispToConsole("No ray was generated?!", ccMainAppInterface::WRN_CONSOLE_MESSAGE);
		return;
	}

	// Run ShadeVis
	ccProgressDialog pcvProgressCb(true, m_app->getMainWindow());
	pcvProgressCb.setAutoClose(false);

	QElapsedTimer timer;
	timer.start();
	PCVCommand::Process(candidates, rays, meshIsClosed, resolution, &pcvProgressCb, m_app);
	ccLog::Print(QString("[PCV] Timing: %1 sec").arg(timer.elapsed() / 1000.0));

	pcvProgressCb.close();
	m_app->updateUI();
	m_app->refreshAll();
}

// qPCV::registerCommands
/**
 * @brief Register command-line commands
 *
 * Registers the -PCV command for batch processing via
 * qCC -SILENT -CMD -O cloud.las -PCV ...
 *
 * @param[in] cmd Command-line interface
 */
void qPCV::registerCommands(ccCommandLineInterface* cmd)
{
	cmd->registerCommand(ccCommandLineInterface::Command::Shared(new PCVCommand));
}
