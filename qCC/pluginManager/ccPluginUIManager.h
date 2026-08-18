#ifndef CCPLUGINUIMANAGER_H
#define CCPLUGINUIMANAGER_H

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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccPluginUIManager.h
 *
 * @brief Plugin UI manager
 *
 * Manages plugin menus, toolbars, and actions.
 *
 * @author CloudCompare project
 */

#include <QActionGroup>
#include <QList>
#include <QObject>

class QAction;
class QMenu;
class QString;
class QToolBar;
class QWidget;

class ccMainAppInterface;
class ccPluginInterface;
class ccStdPluginInterface;

/**
 * @brief Plugin UI manager
 *
 * Manages plugin menus, toolbars, and actions.
 */
class ccPluginUIManager : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Create manager
	 * @param[in] appInterface Application interface
	 * @param[in] parent Parent widget
	 */
	ccPluginUIManager(ccMainAppInterface* appInterface, QWidget* parent);
	
	/// Destructor
	~ccPluginUIManager() override = default;

	/// Initialize manager
	void init();

	/// Get plugin menu
	QMenu* pluginMenu() const;
	/// Get shader and filter menu
	QMenu* shaderAndFilterMenu() const;

	/// Get main plugin toolbar
	QToolBar* mainPluginToolbar();
	/// Get additional plugin toolbars
	QList<QToolBar*>& additionalPluginToolbars();
	/// Get show main toolbar action
	QAction* actionShowMainPluginToolbar();

	/// Get GL filters toolbar
	QToolBar* glFiltersToolbar();
	/// Get show GL filter toolbar action
	QAction* actionShowGLFilterToolbar();

	/// Update menus
	void updateMenus();
	/// Handle selection changed
	void handleSelectionChanged();

	/// Show about dialog
	void showAboutDialog() const;

  private:
	/// Set up actions
	void setupActions();

	/// Set up menus
	void setupMenus();
	/// Add actions to menu
	void addActionsToMenu(ccStdPluginInterface* stdPlugin, const QList<QAction*>& actions);

	/// Set up toolbars
	void setupToolbars();
	/// Add actions to toolbar
	void addActionsToToolBar(ccStdPluginInterface* stdPlugin, const QList<QAction*>& actions);

	/// Enable GL filter
	void enableGLFilter();
	/// Disable GL filter
	void disableGLFilter();

	QWidget* m_parentWidget;

	ccMainAppInterface* m_appInterface;

	QMenu* m_pluginMenu;
	QMenu* m_glFilterMenu;

	QAction* m_actionRemoveFilter;
	// ... more members
};

#endif
