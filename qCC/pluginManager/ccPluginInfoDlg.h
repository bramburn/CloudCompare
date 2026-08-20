#ifndef CCPLUGININFODLG_H
#define CCPLUGININFODLG_H
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
 * @file ccPluginInfoDlg.h
 *
 * @brief Plugin info dialog
 *
 * Dialog for displaying plugin information.
 *
 * @author CloudCompare project
 */

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
 * @brief Plugin info dialog
 *
 * Display information about available plugins.
 */
class ccPluginInfoDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccPluginInfoDlg(QWidget* parent = nullptr);
	
	/// Destructor
	~ccPluginInfoDlg() override;

	/// Set plugin paths
	void setPluginPaths(const QStringList& pluginPaths);
	/// Set plugin list
	void setPluginList(const QList<ccPluginInterface*>& pluginList);

  private:
	/// User role for plugin pointer
	enum
	{
		PLUGIN_PTR = Qt::UserRole + 1
	};

	/// Get plugin from item data
	const ccPluginInterface* pluginFromItemData(const QStandardItem* item) const;

	/// Handle selection changed
	void selectionChanged(const QModelIndex& current, const QModelIndex& previous);
	/// Handle item changed
	void itemChanged(QStandardItem* item);

	/// Update plugin info display
	void updatePluginInfo(const ccPluginInterface* plugin);

	Ui::ccPluginInfoDlg* m_UI;

	QSortFilterProxyModel* m_ProxyModel;
	QStandardItemModel*    m_ItemModel;
};

#endif
