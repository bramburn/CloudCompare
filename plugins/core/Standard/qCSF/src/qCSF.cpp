//#######################################################################################
//#                                                                                     #
//#                              CLOUDCOMPARE PLUGIN: qCSF                              #
//#                                                                                     #
//#        This program is free software; you can redistribute it and/or modify         #
//#        it under the terms of the GNU General Public License as published by         #
//#        the Free Software Foundation; version 2 or later of the License.             #
//#                                                                                     #
//#        This program is distributed in the hope that it will be useful,              #
//#        but WITHOUT ANY WARRANTY; without even the implied warranty of               #
//#        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 #
//#        GNU General Public License for more details.                                 #
//#                                                                                     #
//#        Please cite the following paper, If you use this plugin in your work.        #
//#                                                                                     #
//#  Zhang W, Qi J, Wan P, Wang H, Xie D, Wang X, Yan G. An Easy-to-Use Airborne LiDAR  #
//#  Data Filtering Method Based on Cloth Simulation. Remote Sensing. 2016; 8(6):501.   #
//#                                                                                     #
//#                                     Copyright ©                                     #
//#               RAMM laboratory, School of Geography, Beijing Normal University       #
//#                               (http://ramm.bnu.edu.cn/)                             #
//#                                                                                     #
//#                      Wuming Zhang; Jianbo Qi; Peng Wan; Hongtao Wang                #
//#                                                                                     #
//#                      contact us: 2009zwm@gmail.com; wpqjbzwm@126.com                #
//#                                                                                     #
//#######################################################################################

/**
 * @file qCSF.cpp
 *
 * @brief Cloth Simulation Filter plugin implementation
 *
 * Implements the CSF ground filtering algorithm for CloudCompare.
 * Handles plugin lifecycle, action registration, and cloud splitting.
 *
 * Processing flow:
 * 1. User selects a point cloud and triggers the action
 * 2. Parameter dialog (semi-persistent settings)
 * 3. CSF::Apply() computes ground/non-ground classification
 * 4. Original cloud hidden, two new clouds added to DB tree
 * 5. Optional cloth mesh added for visualization
 *
 * @see qCSF
 */

// A mex version for programming in Matlab is at File Exchange of Mathworks website:
// http://www.mathworks.com/matlabcentral/fileexchange/58139-csf--ground-filtering-of-point-cloud-based-on-cloth-simulation

#include "qCSF.h"

// Qt
#include <QApplication>
#include <QProgressDialog>
#include <QMainWindow>
#include <QComboBox>
#include <QElapsedTimer>

// Dialog
#include "ccCSFDlg.h"

// Local
#include "qCSFCommands.h"

// System
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>

// qCC_db
#include <ccGenericPointCloud.h>
#include <ccPointCloud.h>
#include <ccProgressDialog.h>
#include <ccHObjectCaster.h>
#include <ccOctree.h>
#include <ccMesh.h>

// CSF
#include <CSF.h>

// qCSF::qCSF
/**
 * @brief Construct the CSF plugin
 *
 * Initializes the ccStdPluginInterface with the plugin metadata from
 * the embedded resource (":/CC/plugin/qCSF/info.json").
 */
qCSF::qCSF(QObject* parent)
    : QObject(parent)
    , ccStdPluginInterface(":/CC/plugin/qCSF/info.json")
    , m_action(nullptr)
{
}

// qCSF::onNewSelection
/**
 * @brief Handle selection change
 *
 * Enables the "Cloth Simulation Filter" action if the selection
 * contains at least one point cloud.
 *
 * @param[in] selectedEntities Current entity selection
 */
void qCSF::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action)
	{
		bool hasCloud = false;
		for (ccHObject* entity : selectedEntities)
		{
			if (entity && entity->isA(CC_TYPES::POINT_CLOUD))
			{
				hasCloud = true;
				break;
			}
		}
		m_action->setEnabled(hasCloud);
	}
}

// qCSF::getActions
/**
 * @brief Create and return the plugin action
 *
 * Lazily creates the "Cloth Simulation Filter" action and connects
 * its triggered() signal to doAction().
 *
 * @return List containing the plugin action
 */
QList<QAction*> qCSF::getActions()
{
	if (!m_action)
	{
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());
		connect(m_action, &QAction::triggered, this, &qCSF::doAction);
	}
	return {m_action};
}

// qCSF::doAction
/**
 * @brief Execute the CSF filter
 *
 * Runs the Cloth Simulation Filter on all selected point clouds:
 * 1. Validates selection (requires at least one cloud)
 * 2. Shows parameter dialog (semi-persistent settings)
 * 3. Calls CSF::Apply() for each cloud
 * 4. Creates "Ground points" and "Off-ground points" clouds
 * 5. Optionally adds the simulated cloth mesh
 * 6. Hides the original cloud and adds results to the DB tree
 *
 * Parameter persistence: dialog values are stored as static variables
 * and restored on each invocation for semi-persistent UX.
 */
void qCSF::doAction()
{
	// m_app should have already been initialized by CC when plugin is loaded!
	if (!m_app)
	{
		assert(false);
		return;
	}

	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();
	if (selectedEntities.empty())
	{
		assert(false);
		m_app->dispToConsole("Select at least one cloud", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	// Semi-persistent parameters (restored on each invocation)
	static bool  PostProcessing      = false;
	static double ClothResolution   = 2.0;
	static double ClassThreshold    = 0.5;
	static int    Rigidness        = 2;
	static int    MaxIteration     = 500;
	static bool   ExportClothMesh  = false;

	// Show parameter dialog
	{
		ccCSFDlg csfDlg(m_app->getMainWindow());
		csfDlg.postprocessingcheckbox->setChecked(PostProcessing);
		csfDlg.rig1->setChecked(Rigidness == 1);
		csfDlg.rig2->setChecked(Rigidness == 2);
		csfDlg.rig3->setChecked(Rigidness == 3);
		csfDlg.MaxIterationSpinBox->setValue(MaxIteration);
		csfDlg.cloth_resolutionSpinBox->setValue(ClothResolution);
		csfDlg.class_thresholdSpinBox->setValue(ClassThreshold);
		csfDlg.exportClothMeshCheckBox->setChecked(ExportClothMesh);

		if (!csfDlg.exec())
			return;

		// Persist for next invocation
		PostProcessing     = csfDlg.postprocessingcheckbox->isChecked();
		ClothResolution   = csfDlg.cloth_resolutionSpinBox->value();
		ClassThreshold    = csfDlg.class_thresholdSpinBox->value();
		ExportClothMesh   = csfDlg.exportClothMeshCheckBox->isChecked();
		if (csfDlg.rig1->isChecked())
			Rigidness = 1;
		else if (csfDlg.rig2->isChecked())
			Rigidness = 2;
		else
			Rigidness = 3;
		MaxIteration = csfDlg.MaxIterationSpinBox->value();
	}

	// Build CSF parameters
	CSF::Parameters csfParams;
	{
		csfParams.smoothSlope      = PostProcessing;
		csfParams.class_threshold = ClassThreshold;
		csfParams.cloth_resolution = ClothResolution;
		csfParams.rigidness      = Rigidness;
		csfParams.iterations     = MaxIteration;
	}

	// Show progress dialog
	QProgressDialog pDlg(m_app->getMainWindow());
	pDlg.setWindowTitle("CSF");
	pDlg.setLabelText(tr("Computing...."));
	pDlg.setCancelButton(nullptr);
	pDlg.setRange(0, 0); // indeterminate progress bar
	pDlg.show();
	QApplication::processEvents();

	// Process each selected cloud
	for (ccHObject* ent : selectedEntities)
	{
		if (!ent || !ent->isA(CC_TYPES::POINT_CLOUD))
			continue;

		ccPointCloud* pc = static_cast<ccPointCloud*>(ent);

		ccPointCloud* groundCloud    = nullptr;
		ccPointCloud* offGroundCloud = nullptr;
		ccMesh* clothMesh           = nullptr;

		// Run CSF classification
		if (!CSF::Apply(pc, csfParams, groundCloud, offGroundCloud,
		                ExportClothMesh, clothMesh, m_app))
		{
			m_app->dispToConsole("Not enough memory", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
			return;
		}

		// Hide original cloud
		pc->setEnabled(false);

		// Create container group
		ccHObject* cloudContainer = new ccHObject(pc->getName() + "_CSF");
		if (pc->getParent())
			pc->getParent()->addChild(cloudContainer);

		// Add ground cloud
		if (groundCloud)
		{
			groundCloud->copyGlobalShiftAndScale(*pc);
			groundCloud->setVisible(true);
			groundCloud->setName("Ground points");
			cloudContainer->addChild(groundCloud);
		}

		// Add non-ground cloud
		if (offGroundCloud)
		{
			offGroundCloud->copyGlobalShiftAndScale(*pc);
			offGroundCloud->setVisible(true);
			offGroundCloud->setName("Off-ground points");
			cloudContainer->addChild(offGroundCloud);
		}

		// Add cloth mesh
		if (clothMesh)
		{
			clothMesh->copyGlobalShiftAndScale(*pc);
			clothMesh->getAssociatedCloud()->copyGlobalShiftAndScale(*pc);
			clothMesh->computePerVertexNormals();
			clothMesh->showNormals(true);
			cloudContainer->addChild(clothMesh);
		}

		m_app->addToDB(cloudContainer);
	}

	pDlg.hide();
	QApplication::processEvents();
	m_app->refreshAll();
}

// qCSF::registerCommands
/**
 * @brief Register command-line commands
 *
 * Registers the -CSF command for batch processing via the command-line
 * interface (qCC -SILENT -CMD -O input.las -CSF ...).
 *
 * @param[in] cmd Command-line interface (nullptr = not in CLI mode)
 */
void qCSF::registerCommands(ccCommandLineInterface* cmd)
{
	if (!cmd)
	{
		assert(false);
		return;
	}
	cmd->registerCommand(ccCommandLineInterface::Command::Shared(new CommandCSF));
}
