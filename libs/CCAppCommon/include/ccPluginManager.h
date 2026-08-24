#pragma once
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
 * @file ccPluginManager.h
 *
 * @brief Plugin manager for CloudCompare
 *
 * Manages discovery, loading, and lifecycle of CloudCompare plugins.
 *
 * Architecture:
 * - Plugins are discovered by scanning directories in m_pluginPaths
 * - Each directory is searched for .dll (Windows) or .so/.dylib (Unix) files
 * - Each library is loaded via QPluginLoader and queried for ccPluginInterface
 * - I/O filters are registered with FileIOFilter::Register()
 * - Standard and GL plugins are stored in m_pluginList for action access
 *
 * Plugin types (ccPluginInterface::GetType):
 * - IO (ccIOPluginInterface): file format readers/writers
 * - Standard (ccStdPluginInterface): analysis tools, add actions to Plugins menu
 * - GL (ccGLPluginInterface): OpenGL post-processing effects
 *
 * Enable/disable: plugins can be disabled via the Plugin Manager dialog.
 * Disabled plugins are stored by IID in QSettings and skipped on load.
 *
 * Singleton: accessed via Get() from anywhere in the app.
 */

#include "CCAppCommon.h"

#include <QObject>
#include <QVector>

class ccPluginInterface;

/// Ordered list of loaded plugin interfaces
using ccPluginInterfaceList = QVector<ccPluginInterface*>;

/**
 * @class ccPluginManager
 *
 * @brief Singleton plugin registry and loader
 *
 * Manages discovery and loading of CloudCompare plugins from disk,
 * maintains the list of active plugins, and provides enable/disable
 * functionality persisted to QSettings.
 *
 * Loading sequence (loadPlugins):
 * 1. Clear any previously loaded plugins
 * 2. Get list of disabled plugin IIDs from QSettings
 * 3. For each path in m_pluginPaths: scan for .dll/.so/.dylib files
 * 4. For each library: try QPluginLoader::load()
 * 5. Cast to ccPluginInterface
 * 6. Skip if IID is in disabled list
 * 7. Register I/O filters if applicable (FileIOFilter::Register)
 * 8. Append to m_pluginList
 *
 * @extends QObject (to receive plugin unload signals if needed)
 */
class CCAPPCOMMON_LIB_API ccPluginManager : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Destructor
	 */
	~ccPluginManager() override = default;

	/**
	 * @brief Get the singleton instance
	 *
	 * Creates the instance on first call (lazy singleton).
	 *
	 * @return Reference to the plugin manager
	 */
	static ccPluginManager& Get();

	/**
	 * @brief Set directories to search for plugins
	 *
	 * Called during app initialization before loadPlugins().
	 * The default paths are set by the application (typically
	 * the app directory + plugins/ subdirectory).
	 *
	 * @param[in] paths List of absolute directory paths
	 */
	void setPaths(const QStringList& paths);

	/**
	 * @brief Get the current plugin search paths
	 *
	 * @return List of paths that will be searched
	 */
	QStringList pluginPaths() const;

	/**
	 * @brief Load all plugins from the search paths
	 *
	 * Discovers and loads all compatible plugins, skipping disabled ones.
	 * Safe to call multiple times (clears previous state each time).
	 *
	 * @see setPaths() to set where to search
	 */
	void loadPlugins();

	/**
	 * @brief Get all loaded plugins
	 *
	 * Returns all plugins including Standard, IO, and GL types.
	 * After loadPlugins(), this contains every successfully loaded
	 * (and not disabled) plugin.
	 *
	 * @return Reference to the plugin list
	 */
	ccPluginInterfaceList& pluginList();

	/**
	 * @brief Enable or disable a specific plugin
	 *
	 * Marks a plugin as enabled or disabled. Disabled plugins are
	 * skipped on future loadPlugins() calls by checking their IID.
	 * The enabled state is persisted to QSettings.
	 *
	 * @param[in] plugin Plugin to modify (must be in pluginList)
	 * @param[in] enabled true = load normally, false = skip on load
	 */
	void setPluginEnabled(const ccPluginInterface* plugin, bool enabled);

	/**
	 * @brief Check if a plugin is enabled
	 *
	 * @param[in] plugin Plugin to check
	 * @return true if enabled (or not yet in the disabled list)
	 */
	bool isEnabled(const ccPluginInterface* plugin) const;

  protected:
	/**
	 * @brief Construct the plugin manager
	 *
	 * @param[in] parent QObject parent
	 */
	explicit ccPluginManager(QObject* parent = nullptr);

  private:
	/**
	 * @brief Scan paths, load libraries, add to m_pluginList
	 *
	 * Internal implementation of loadPlugins(). Uses QPluginLoader
	 * for each discovered library file.
	 */
	void loadFromPathsAndAddToList();

	/**
	 * @brief Read disabled plugin IIDs from QSettings
	 *
	 * Reads the "DisabledPlugins" key from the CloudCompare settings
	 * group.
	 *
	 * @param[out] disabledPlugins Output list (cleared and repopulated)
	 */
	void getDisabledPluginIIDs(QStringList& disabledPlugins) const;

  private:
	//! Directories searched for plugins
	QStringList m_pluginPaths;
	//! Successfully loaded plugins (all types)
	ccPluginInterfaceList m_pluginList;
};
