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
// #          COPYRIGHT: CloudCompare project                               //
// #                                                                        #
// ##########################################################################

/**
 * @file ccPluginUIManager.h
 *
 * @brief Plugin UI manager for CloudCompare.
 *
 * @details Manages the integration of plugin-provided UI elements (menus,
 * toolbars, actions) into the CloudCompare interface. This class:
 * - Creates plugin menus and submenus
 * - Manages plugin toolbar placement
 * - Handles GL filter toolbar management
 * - Forwards selection change events to plugins
 *
 * @author CloudCompare project
 *
 * @see ccPluginInterface
 * @see ccStdPluginInterface
 * @see MainWindow
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
 * @brief Manages plugin UI integration.
 *
 * @details The ccPluginUIManager class coordinates the integration of
 * plugins into the CloudCompare user interface. It:
 * - Creates menus for standard and I/O plugins
 * - Manages toolbar placement for plugin actions
 * - Handles GL filter (post-processing) toolbar
 * - Routes selection change events to plugins
 *
 * Plugins can provide:
 * - Menu actions (appear in Plugins menu)
 * - Toolbar actions (appear in plugin toolbars)
 * - GL filters (appear in GL Filters toolbar)
 *
 * @extends QObject
 *
 * @par Usage
 * @code
 * ccPluginUIManager* manager = new ccPluginUIManager(app, mainWindow);
 * manager->init();  // Initialize menus and toolbars
 * @endcode
 *
 * @see MainWindow::initPlugins()
 */
class ccPluginUIManager : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the plugin UI manager.
	 *
	 * @param[in] appInterface Application interface for plugin callbacks.
	 * @param[in] parent Parent widget (typically the main window).
	 */
	ccPluginUIManager(ccMainAppInterface* appInterface, QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	~ccPluginUIManager() override = default;

	/**
	 * @brief Initialize the plugin UI.
	 *
	 * @details Sets up menus, toolbars, and loads plugin actions.
	 * Called once during application startup after plugins are loaded.
	 */
	void init();

	/**
	 * @brief Get the main plugins menu.
	 *
	 * @return Pointer to the Plugins menu.
	 *
	 * @note This menu contains submenus for each standard plugin.
	 */
	QMenu* pluginMenu() const;

	/**
	 * @brief Get the shader and filter menu.
	 *
	 * @return Pointer to the Shaders & Filters menu.
	 */
	QMenu* shaderAndFilterMenu() const;

	/**
	 * @brief Get the main plugin toolbar.
	 *
	 * @return Pointer to the toolbar containing single-action plugins.
	 *
	 * @note Plugins with only one action appear in this toolbar.
	 */
	QToolBar* mainPluginToolbar();

	/**
	 * @brief Get additional plugin toolbars.
	 *
	 * @return Reference to list of additional toolbars.
	 *
	 * @note Plugins with multiple actions get their own toolbar.
	 */
	QList<QToolBar*>& additionalPluginToolbars();

	/**
	 * @brief Get action to show/hide main plugin toolbar.
	 *
	 * @return Action for toggling main plugin toolbar visibility.
	 */
	QAction* actionShowMainPluginToolbar();

	/**
	 * @brief Get the GL filters toolbar.
	 *
	 * @return Pointer to the GL Filters toolbar.
	 */
	QToolBar* glFiltersToolbar();

	/**
	 * @brief Get action to show/hide GL filter toolbar.
	 *
	 * @return Action for toggling GL filter toolbar visibility.
	 */
	QAction* actionShowGLFilterToolbar();

	/**
	 * @brief Update all plugin menus.
	 *
	 * @details Called when selection changes or plugins are loaded.
	 * Enables/disables actions based on current selection.
	 */
	void updateMenus();

	/**
	 * @brief Handle selection change.
	 *
	 * @details Forwards selection change events to plugins so they
	 * can update their action states.
	 */
	void handleSelectionChanged();

	/**
	 * @brief Show the plugin about dialog.
	 *
	 * @details Displays information about all loaded plugins.
	 */
	void showAboutDialog() const;

  private:
	/**
	 * @brief Set up plugin actions.
	 *
	 * @details Loads actions from all plugins.
	 */
	void setupActions();

	/**
	 * @brief Set up plugin menus.
	 *
	 * @details Creates the menu structure and adds plugin submenus.
	 */
	void setupMenus();

	/**
	 * @brief Add actions from a standard plugin to a menu.
	 *
	 * @param[in] stdPlugin Plugin to get actions from.
	 * @param[in] actions Actions to add.
	 */
	void addActionsToMenu(ccStdPluginInterface* stdPlugin, const QList<QAction*>& actions);

	/**
	 * @brief Set up plugin toolbars.
	 *
	 * @details Creates toolbars and adds plugin actions.
	 */
	void setupToolbars();

	/**
	 * @brief Add actions from a standard plugin to a toolbar.
	 *
	 * @param[in] stdPlugin Plugin to get actions from.
	 * @param[in] actions Actions to add.
	 */
	void addActionsToToolBar(ccStdPluginInterface* stdPlugin, const QList<QAction*>& actions);

	/**
	 * @brief Enable the current GL filter.
	 */
	void enableGLFilter();

	/**
	 * @brief Disable all GL filters.
	 */
	void disableGLFilter();

	//! Parent widget for creating new menus/toolbars
	QWidget* m_parentWidget;

	//! Application interface for plugin callbacks
	ccMainAppInterface* m_appInterface;

	//! Main plugins menu
	QMenu* m_pluginMenu;

	//! GL filter menu
	QMenu* m_glFilterMenu;

	//! Action to remove current GL filter
	QAction* m_actionRemoveFilter;

	//! Group for GL filter radio actions
	QActionGroup m_glFilterActions;

	//! List of loaded plugins
	QList<ccPluginInterface*> m_plugins;

	//! Main toolbar for single-action plugins
	QToolBar* m_mainPluginToolbar;

	//! Additional toolbars for multi-action plugins
	QList<QToolBar*> m_additionalPluginToolbars;

	//! Action to toggle main plugin toolbar
	QAction* m_showPluginToolbar;

	//! GL Filters toolbar
	QToolBar* m_glFiltersToolbar;

	//! Action to toggle GL filter toolbar
	QAction* m_showGLFilterToolbar;
};

#endif
