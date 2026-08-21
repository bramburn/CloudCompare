//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qM3C2                       #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#            COPYRIGHT: UNIVERSITE EUROPEENNE DE BRETAGNE                #
//#                                                                        #
//##########################################################################

/**
 * @file qM3C2.cpp
 *
 * @brief M3C2 plugin implementation
 *
 * Implements the Multiscale Model to Model Cloud Comparison (M3C2) algorithm.
 * Handles plugin lifecycle, selection validation, and M3C2 computation.
 *
 * ## M3C2 Algorithm Overview
 *
 * M3C2 (Lague et al. 2013) compares two point clouds without explicit
 * registration. For each point in the core cloud, it:
 * 1. Computes the local normal direction using a neighborhood at "normal scale" D
 * 2. Casts two cylinders along the normal (±): coregistration cloud is sampled,
 *    reference cloud is projected onto the axis
 * 3. Computes mean position of points in each cylinder
 * 4. Distance = difference in mean positions; uncertainty = standard deviation
 *
 * Multi-scale: tests multiple "search radii" and outputs:
 * - M3C2 distance per point
 * - Standard deviation per point (uncertainty)
 * - Point density per point
 * - Normal direction per point
 *
 * ## Processing Flow
 *
 * 1. User selects 2-3 point clouds and triggers the action
 * 2. Disclaimer dialog shown on first use (saved to persistent settings)
 * 3. Parameter dialog (normal scale, search scales, max distance, projection mode)
 * 4. qM3C2Process::Compute() runs the comparison
 * 5. Output cloud with M3C2 distance scalar field added to DB
 *
 * ## Selection Requirements
 *
 * - 2 clouds: compare cloud1 (core) against cloud2 (reference)
 * - 3 clouds: compare cloud1 against cloud3, with cloud2 as reference for normals
 *
 * ## Command-line Mode
 *
 * The -M3C2 command enables batch processing via qCC -SILENT -CMD.
 * Selection is passed via -O file1.las -O file2.las.
 *
 * @see qM3C2Plugin
 * @see qM3C2Process
 * @see qM3C2Dialog
 */

#include "qM3C2.h"

// Qt
#include <QMainWindow>

// local
#include "qM3C2Tools.h"
#include "qM3C2Dialog.h"
#include "qM3C2DisclaimerDialog.h"
#include "qM3C2Commands.h"
#include "qM3C2Process.h"

// qCC_db
#include <ccPointCloud.h>

// qM3C2Plugin::qM3C2Plugin
/**
 * @brief Construct the M3C2 plugin
 *
 * Initializes ccStdPluginInterface with embedded metadata
 * from the resource path ":/CC/plugin/qM3C2Plugin/info.json".
 */
qM3C2Plugin::qM3C2Plugin(QObject* parent)
    : QObject(parent)
    , ccStdPluginInterface(":/CC/plugin/qM3C2Plugin/info.json")
    , m_action(nullptr)
{
}

// qM3C2Plugin::onNewSelection
/**
 * @brief Handle selection change
 *
 * Enables the "M3C2" action when exactly 2 or 3 point clouds are selected.
 * Selection is cached in m_selectedEntities for use in doAction().
 *
 * Selection rules:
 * - 2 clouds: coregistration mode (cloud1 vs cloud2)
 * - 3 clouds: two comparisons sharing cloud2 as the normal reference
 *
 * @param[in] selectedEntities Current entity selection
 */
void qM3C2Plugin::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action)
	{
		bool twoClouds = selectedEntities.size() == 2
			&& selectedEntities[0]->isA(CC_TYPES::POINT_CLOUD)
			&& selectedEntities[1]->isA(CC_TYPES::POINT_CLOUD);

		bool threeClouds = selectedEntities.size() == 3
			&& selectedEntities[0]->isA(CC_TYPES::POINT_CLOUD)
			&& selectedEntities[1]->isA(CC_TYPES::POINT_CLOUD)
			&& selectedEntities[2]->isA(CC_TYPES::POINT_CLOUD);

		m_action->setEnabled(twoClouds || threeClouds);
	}
	m_selectedEntities = selectedEntities;
}

// qM3C2Plugin::getActions
/**
 * @brief Create and return the plugin action
 *
 * Lazily creates the "M3C2" action (shown in the Plugins menu) and
 * connects its triggered() signal to doAction().
 *
 * @return List containing the M3C2 action
 */
QList<QAction*> qM3C2Plugin::getActions()
{
	if (!m_action)
	{
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());
		connect(m_action, &QAction::triggered, this, &qM3C2Plugin::doAction);
	}
	return QList<QAction*>{m_action};
}

// qM3C2Plugin::doAction
/**
 * @brief Execute the M3C2 comparison
 *
 * Full workflow:
 * 1. Show disclaimer dialog (first-use guard; acceptance saved to QSettings)
 * 2. Validate cached selection (2 or 3 point clouds)
 * 3. Cast entities to ccPointCloud pointers
 * 4. Show qM3C2Dialog for parameters:
 *    - Normal scale D (neighborhood radius for normal estimation)
 *    - Search scale(s) (cylinder radius, may be multi-scale)
 *    - Max search distance (truncation distance)
 *    - Projection mode, detrending, etc.
 * 5. Call qM3C2Process::Compute() for the actual algorithm
 * 6. On success: results added to DB automatically by Compute()
 * 7. On failure: display error via dispToConsole()
 * 8. Persist dialog parameters for next invocation
 *
 * @note Selection is cached by onNewSelection() to avoid re-querying
 *       the application during dialog interaction.
 */
void qM3C2Plugin::doAction()
{
	// Disclaimer required on first use
	if (!DisclaimerDialog::show(m_app))
		return;

	assert(m_app);
	if (!m_app)
		return;

	// Validate cached selection (2 or 3 clouds)
	bool twoClouds = m_selectedEntities.size() == 2
		&& m_selectedEntities[0]->isA(CC_TYPES::POINT_CLOUD)
		&& m_selectedEntities[1]->isA(CC_TYPES::POINT_CLOUD);

	bool threeClouds = m_selectedEntities.size() == 3
		&& m_selectedEntities[0]->isA(CC_TYPES::POINT_CLOUD)
		&& m_selectedEntities[1]->isA(CC_TYPES::POINT_CLOUD)
		&& m_selectedEntities[2]->isA(CC_TYPES::POINT_CLOUD);

	if (!twoClouds && !threeClouds)
	{
		m_app->dispToConsole("Select two or three point clouds!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	// Cast to point clouds
	ccPointCloud* cloud1 = ccHObjectCaster::ToPointCloud(m_selectedEntities[0]);
	ccPointCloud* cloud2 = ccHObjectCaster::ToPointCloud(m_selectedEntities[1]);
	ccPointCloud* cloud3 = threeClouds ? ccHObjectCaster::ToPointCloud(m_selectedEntities[2]) : nullptr;

	// Show parameter dialog
	qM3C2Dialog dlg(cloud1, cloud2, m_app, cloud3);
	if (!dlg.exec())
		return; // cancelled by user

	// Run M3C2 computation
	QString errorMessage;
	ccPointCloud* outputCloud = nullptr; // only populated in CLI mode
	if (!qM3C2Process::Compute(dlg, errorMessage, outputCloud, true,
	                            m_app->getMainWindow(), m_app))
	{
		m_app->dispToConsole(errorMessage, ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}

	// Persist dialog params for next invocation
	dlg.saveParamsToPersistentSettings();
}

// qM3C2Plugin::registerCommands
/**
 * @brief Register command-line commands
 *
 * Registers the -M3C2 command for batch processing via the command-line
 * interface. Usage:
 * @code
 * qCC -SILENT -CMD -O cloud1.las -O cloud2.las -M3C2 [options]
 * @endcode
 *
 * @param[in] cmd Command-line interface (nullptr = not in CLI mode, skip)
 */
void qM3C2Plugin::registerCommands(ccCommandLineInterface* cmd)
{
	if (!cmd)
	{
		assert(false);
		return;
	}
	cmd->registerCommand(ccCommandLineInterface::Command::Shared(new CommandM3C2));
}
