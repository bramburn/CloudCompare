// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPluginUIManager.h
 *
 * @brief Plugin UI manager for integrating plugins with the application UI.
 *
 * @details Manages plugin menus, toolbars, and actions in the CloudCompare
 * main window.
 *
 * ## Overview
 *
 * ccPluginUIManager handles:
 * - Plugin menu integration
 * - Plugin toolbar management
 * - GL filter toolbar
 * - Plugin action routing
 * - Plugin about dialogs
 *
 * ## Architecture
 *
 * Plugins register their actions through this manager, which:
 * 1. Creates dedicated menus for Standard plugins
 * 2. Adds GL filters to the GL filter toolbar
 * 3. Manages toolbar visibility
 * 4. Routes selection changes to plugins
 *
 * ## Usage
 *
 * @code
 * // Initialize plugin UI manager
 * ccPluginUIManager* uiManager = new ccPluginUIManager(appInterface, parent);
 * uiManager->init();
 *
 * // Access plugin menus
 * QMenu* pluginMenu = uiManager->pluginMenu();
 * QToolBar* toolbar = uiManager->mainPluginToolbar();
 *
 * // Handle selection changes
 * uiManager->handleSelectionChanged();
 * @endcode
 *
 * @extends QObject
 *
 * @author CloudCompare project
 *
 * @see ccStdPluginInterface for plugin interface
 * @see ccMainAppInterface for application interface
 */

#ifndef CCPLUGINUIMANAGER_H
#define CCPLUGINUIMANAGER_H

// Qt
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
 * @brief Plugin UI manager.
 *
 * @details Manages plugin integration with the CloudCompare UI.
 */
class ccPluginUIManager : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Create plugin UI manager.
	 *
	 * @param[in] appInterface Application interface.
	 * @param[in] parent Parent widget.
	 */
	ccPluginUIManager(ccMainAppInterface* appInterface, QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	~ccPluginUIManager() override = default;

	/**
	 * @brief Initialize plugin UI.
	 *
	 * Sets up menus, toolbars, and loads plugins.
	 */
	void init();

	/**
	 * @brief Get plugin menu.
	 *
	 * @return Plugin menu.
	 */
	QMenu* pluginMenu() const;

	/**
	 * @brief Get shader and filter menu.
	 *
	 * @return Shader and filter menu.
	 */
	QMenu* shaderAndFilterMenu() const;

	/**
	 * @brief Get main plugin toolbar.
	 *
	 * @return Main plugin toolbar.
	 */
	QToolBar* mainPluginToolbar();

	/**
	 * @brief Get additional plugin toolbars.
	 *
	 * @return List of additional toolbars.
	 */
	QList<QToolBar*>& additionalPluginToolbars();

	/**
	 * @brief Get show main plugin toolbar action.
	 *
	 * @return Toolbar toggle action.
	 */
	QAction* actionShowMainPluginToolbar();

	/**
	 * @brief Get GL filters toolbar.
	 *
	 * @return GL filters toolbar.
	 */
	QToolBar* glFiltersToolbar();

	/**
	 * @brief Get show GL filter toolbar action.
	 *
	 * @return Toolbar toggle action.
	 */
	QAction* actionShowGLFilterToolbar();

	/**
	 * @brief Update all plugin menus.
	 */
	void updateMenus();

	/**
	 * @brief Handle selection changed.
	 *
	 * Notifies plugins of selection changes.
	 */
	void handleSelectionChanged();

	/**
	 * @brief Show about dialog.
	 *
	 * Displays plugin information dialog.
	 */
	void showAboutDialog() const;

  private:
	/**
	 * @brief Set up plugin actions.
	 */
	void setupActions();

	/**
	 * @brief Set up plugin menus.
	 */
	void setupMenus();

	/**
	 * @brief Add actions to plugin menu.
	 *
	 * @param[in] stdPlugin Standard plugin.
	 * @param[in] actions Actions to add.
	 */
	void addActionsToMenu(ccStdPluginInterface* stdPlugin, const QList<QAction*>& actions);

	/**
	 * @brief Set up plugin toolbars.
	 */
	void setupToolbars();

	/**
	 * @brief Add actions to toolbar.
	 *
	 * @param[in] stdPlugin Standard plugin.
	 * @param[in] actions Actions to add.
	 */
	void addActionsToToolBar(ccStdPluginInterface* stdPlugin, const QList<QAction*>& actions);

	/**
	 * @brief Enable GL filter.
	 */
	void enableGLFilter();

	/**
	 * @brief Disable GL filter.
	 */
	void disableGLFilter();

  private:
	//! Parent widget.
	QWidget* m_parentWidget;

	//! Application interface.
	ccMainAppInterface* m_appInterface;

	//! Plugin menu.
	QMenu* m_pluginMenu;

	//! GL filter menu.
	QMenu* m_glFilterMenu;

	//! Remove filter action.
	QAction* m_actionRemoveFilter;

	//! Plugins list.
	QList<ccPluginInterface*> m_plugins;

  private:
	//! Main plugin toolbar.
	QToolBar* m_mainPluginToolbar;

	//! GL filter toolbar.
	QToolBar* m_glFiltersToolbar;

	//! Show main toolbar action.
	QAction* m_actionShowMainPluginToolbar;

	//! Show GL toolbar action.
	QAction* m_actionShowGLFilterToolbar;

	//! Show plugin toolbar action.
	QAction* m_showPluginToolbar;

	//! Show GL filter toolbar action.
	QAction* m_showGLFilterToolbar;

	//! GL filter actions group.
	QActionGroup* m_glFilterActions;

	//! Additional plugin toolbars.
	QList<QToolBar*> m_additionalPluginToolbars;
};

#endif // CCPLUGINUIMANAGER_H
