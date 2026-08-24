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
// #                    COPYRIGHT: Daniel Girardeau-Montaut                 #
// #                                                                        #
// ##########################################################################

#ifndef CC_ITEM_SELECTION_DIALOG_HEADER
#define CC_ITEM_SELECTION_DIALOG_HEADER

/**
 * @file ccItemSelectionDlg.h
 *
 * @brief Item selection dialog
 *
 * Generic dialog for selecting items from a list.
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ui_itemSelectionDlg.h>

// qCC_db
#include <ccHObject.h>

/**
 * @brief Item selection dialog
 *
 * Select one or multiple items from a list.
 */
class ccItemSelectionDlg : public QDialog
    , public Ui::ItemSelectionDlg
{
	Q_OBJECT

  public: // static shortcuts
	/**
	 * @brief Select single entity
	 * @param[in] entities Available entities
	 * @param[in] defaultSelectedIndex Default selected index
	 * @param[in] parent Parent widget
	 * @param[in] label Dialog label
	 * @return Selected index
	 */
	static int SelectEntity(const ccHObject::Container& entities,
	                        int defaultSelectedIndex = 0,
	                        QWidget* parent = nullptr,
	                        QString label = QString());

	/**
	 * @brief Select multiple entities
	 * @param[in] entities Available entities
	 * @param[out] indexes Selected indexes
	 * @param[in] parent Parent widget
	 * @param[in] label Dialog label
	 * @return true if selection was made
	 */
	static bool SelectEntities(const ccHObject::Container& entities,
	                           std::vector<int>& indexes,
	                           QWidget* parent = nullptr,
	                           QString label = QString());

  public:
	/**
	 * @brief Create dialog
	 * @param[in] multiSelectionEnabled Allow multiple selection
	 * @param[in] parent Parent widget
	 * @param[in] itemName Item name
	 * @param[in] label Dialog label
	 */
	ccItemSelectionDlg(bool multiSelectionEnabled,
	                   QWidget* parent = nullptr,
	                   QString itemName = "entities",
	                   QString label = QString());

	/**
	 * @brief Set items
	 * @param[in] items List of items
	 * @param[in] defaultSelectedIndex Default selected index
	 */
	void setItems(const QStringList& items, int defaultSelectedIndex = 0);

	/**
	 * @brief Get selected index (single selection)
	 * @return Selected index
	 */
	int getSelectedIndex() const;

	/**
	 * @brief Get selected indexes (multi selection)
	 * @param[out] indexes Selected indexes
	 */
	void getSelectedIndexes(std::vector<int>& indexes) const;
};

#endif // CC_ITEM_SELECTION_DIALOG_HEADER
