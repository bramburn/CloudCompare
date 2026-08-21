//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qM3C2                       #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
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
 * Processing flow:
 * 1. User selects 2-3 point clouds and triggers the action
 * 2. Disclaimer dialog shown on first use (saved to settings)
 * 3. Parameter dialog (normal scale, search scales, max distance)
 * 4. qM3C2Process::Compute() runs the comparison
 * 5. Output cloud with M3C2 distance scalar field added to DB
 *
 * Selection requirements:
 * - 2 clouds: coregistration comparison
 * - 3 clouds: two comparisons with shared reference cloud
 *
 * @see qM3C2Plugin
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
 * (":/CC/plugin/qM3C2Plugin/info.json").
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
 * Lazily creates the "M3C2" action and connects triggered() to doAction().
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
 * Workflow:
 * 1. Show disclaimer dialog (first-use guard, saves acceptance to settings)
 * 2. Validate cached selection (2 or 3 point clouds)
 * 3. Cast entities to ccPointCloud pointers
 * 4. Show parameter dialog (normal scale, search radii, max distance)
 * 5. Call qM3C2Process::Compute() for the actual algorithm
 * 6. Display results or error message
 * 7. Save dialog params for next invocation
 *
 * @note Selection is cached by onNewSelection() to avoid re-querying
 *       the app during dialog interaction.
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
	ccPointCloud* outputCloud = nullptr; // only needed for CLI mode
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
 * interface (qCC -SILENT -CMD -O cloud1.las -O cloud2.las -M3C2 ...).
 *
 * @param[in] cmd Command-line interface (nullptr = not in CLI mode)
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
