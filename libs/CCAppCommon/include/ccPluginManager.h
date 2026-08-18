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
 * Manages loading and lifecycle of plugins.
 *
 * @author CloudCompare project
 */

#include "CCAppCommon.h"

#include <QObject>
#include <QVector>

class ccPluginInterface;

/// List of plugin interfaces
using ccPluginInterfaceList = QVector<ccPluginInterface*>;

/**
 * @brief Plugin manager singleton
 *
 * Manages plugin loading and lifecycle.
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
	 * @brief Get singleton instance
	 * @return Plugin manager instance
	 */
	static ccPluginManager& Get();

	/**
	 * @brief Set plugin search paths
	 * @param[in] paths Directories to search for plugins
	 */
	void setPaths(const QStringList& paths);
	
	/**
	 * @brief Get plugin search paths
	 * @return List of paths
	 */
	QStringList pluginPaths() const;

	/**
	 * @brief Load all plugins from search paths
	 */
	void loadPlugins();

	/**
	 * @brief Get loaded plugins
	 * @return List of plugins
	 */
	ccPluginInterfaceList& pluginList();

	/**
	 * @brief Enable or disable a plugin
	 * @param[in] plugin Plugin to modify
	 * @param[in] enabled Enable state
	 */
	void setPluginEnabled(const ccPluginInterface* plugin, bool enabled);
	
	/**
	 * @brief Check if plugin is enabled
	 * @param[in] plugin Plugin to check
	 * @return true if enabled
	 */
	bool isEnabled(const ccPluginInterface* plugin) const;

  protected:
	/**
	 * @brief Create plugin manager
	 * @param[in] parent Parent object
	 */
	explicit ccPluginManager(QObject* parent = nullptr);

  private: // methods
	/// Load plugins from paths
	void loadFromPathsAndAddToList();

	/// Get list of disabled plugin IIDs
	void getDisabledPluginIIDs(QStringList& disabledPlugins) const;

  private: // members
	/// Plugin search paths
	QStringList m_pluginPaths;
	/// Loaded plugins
	ccPluginInterfaceList m_pluginList;
};
