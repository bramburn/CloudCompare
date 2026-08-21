//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qBroom                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#      COPYRIGHT: Wesley Grimes (Collision Engineering Associates)       #
//#                                                                        #
//##########################################################################

/**
 * @file qBroom.cpp
 *
 * @brief Virtual broom plugin implementation
 *
 * Implements the qBroom plugin lifecycle.
 *
 * Processing flow:
 * 1. Show disclaimer (first-use guard via ShowDisclaimer)
 * 2. Validate selection (one point cloud)
 * 3. Create qBroomDlg (passes the cloud to it)
 * 4. Show dialog (non-modal so user can interact with 3D view)
 * 5. Deselect the input cloud (so it doesn't interfere with broom interaction)
 * 6. Enter exec() loop — qBroomDlg handles the actual broom logic
 *
 * @see qBroom
 */

#include "qBroom.h"
#include "qBroomDlg.h"
#include "qBroomDisclaimerDialog.h"

//Qt
#include <QtGui>

//qCC_db
#include <ccPointCloud.h>

//qCC
#include <ccGLWindowInterface.h>

//system
#include <assert.h>

qBroom::qBroom(QObject* parent)
	: QObject( parent )
	, ccStdPluginInterface( ":/CC/plugin/qBroom/info.json" )
	, m_action( nullptr )
{
}

// qBroom::getActions
/**
 * @brief Create and return the plugin action
 *
 * Lazily creates the "Broom" action and connects triggered() to doAction().
 *
 * @return List containing the broom action
 */
QList<QAction*> qBroom::getActions()
{
	if (!m_action)
	{
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());
		connect(m_action, &QAction::triggered, this, &qBroom::doAction);
	}
	return QList<QAction*>{m_action};
}

void qBroom::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action)
	{
		//a single point cloud must be selected
		m_action->setEnabled(selectedEntities.size() == 1 && selectedEntities.front()->isA(CC_TYPES::POINT_CLOUD));
	}
}

// qBroom::doAction
/**
 * @brief Execute the broom tool
 *
 * Workflow:
 * 1. Show disclaimer dialog (first-use only)
 * 2. Validate: one point cloud selected
 * 3. Create qBroomDlg, show it non-modally
 * 4. Deselect input cloud (prevents interference with brush interaction)
 * 5. Pass cloud to dialog and enter exec() loop
 * 6. Refresh all views after completion
 */
void qBroom::doAction()
{
	if (!m_app)
	{
		assert(false);
		return;
	}

	//disclaimer accepted?
	if (!ShowDisclaimer(m_app))
	{
		return;
	}

	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();

	if ( !m_app->haveOneSelection() || !selectedEntities.front()->isA(CC_TYPES::POINT_CLOUD))
	{
		m_app->dispToConsole("Select one cloud!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	ccPointCloud* cloud = static_cast<ccPointCloud*>(selectedEntities.front());

	qBroomDlg broomDlg(m_app);
	
	//the widget should be visible before we add the cloud
	broomDlg.show();
	QCoreApplication::processEvents();
	
	//automatically deselect the input cloud
	m_app->setSelectedInDB(cloud, false);

	if (broomDlg.setCloud(cloud))
	{
		broomDlg.exec();
	}

	//currently selected entities appearance may have changed!
	m_app->refreshAll();
}
