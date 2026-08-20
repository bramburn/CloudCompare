// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPluginInfoDlg.h
 *
 * @brief Plugin information display dialog.
 *
 * @details Dialog for displaying information about available CloudCompare
 * plugins including their names, types, descriptions, and authors.
 *
 * Features:
 * - List of all available plugins
 * - Enable/disable plugin toggles
 * - Plugin metadata display
 * - Search/filter functionality
 *
 * @author CloudCompare project
 *
 * @see ccPluginUIManager for plugin management
 * @see ccPluginInterface for plugin interface
 */

#ifndef CCPLUGININFODLG_H
#define CCPLUGININFODLG_H

#include <QDialog>
#include <QList>

class QModelIndex;
class QSortFilterProxyModel;
class QStandardItem;
class QStandardItemModel;

class ccPluginInterface;

namespace Ui
{
	class ccPluginInfoDlg;
}

/**
 * @brief Plugin information dialog.
 *
 * @details Displays a list of available plugins with their metadata
 * and allows toggling their enabled state.
 *
 * Features:
 * - Scrollable plugin list
 * - Enable/disable toggles
 * - Plugin details panel
 * - Search filtering
 *
 * @extends QDialog
 */
class ccPluginInfoDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the plugin info dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccPluginInfoDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccPluginInfoDlg() override;

	/**
	 * @brief Set the plugin search paths.
	 *
	 * @param[in] pluginPaths Directories to search for plugins.
	 */
	void setPluginPaths(const QStringList& pluginPaths);

	/**
	 * @brief Set the loaded plugin list.
	 *
	 * @param[in] pluginList List of loaded plugins.
	 */
	void setPluginList(const QList<ccPluginInterface*>& pluginList);

  private:
	/**
	 * @brief User role for plugin pointer storage.
	 */
	enum
	{
		PLUGIN_PTR = Qt::UserRole + 1
	};

	/**
	 * @brief Get plugin from item data.
	 *
	 * @param[in] item Standard item.
	 *
	 * @return Plugin interface.
	 */
	const ccPluginInterface* pluginFromItemData(const QStandardItem* item) const;

	/**
	 * @brief Handle selection change.
	 *
	 * @param[in] current Current index.
	 * @param[in] previous Previous index.
	 */
	void selectionChanged(const QModelIndex& current, const QModelIndex& previous);

	/**
	 * @brief Handle item change.
	 *
	 * @param[in] item Changed item.
	 */
	void itemChanged(QStandardItem* item);

	/**
	 * @brief Update plugin info display.
	 *
	 * @param[in] plugin Selected plugin.
	 */
	void updatePluginInfo(const ccPluginInterface* plugin);

  private:
	//! UI definition
	Ui::ccPluginInfoDlg* m_UI;

	//! Proxy model for filtering
	QSortFilterProxyModel* m_ProxyModel;

	//! Item model for plugin list
	QStandardItemModel* m_ItemModel;
};

#endif
