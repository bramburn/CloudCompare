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
 * @brief Generic dialog for selecting items from a list.
 *
 * @details A reusable dialog for selecting one or more items
 * from a listbox. Can be used in two modes:
 *
 * - **Single selection**: Returns one selected index
 * - **Multi selection**: Returns multiple selected indexes
 *
 * Static convenience methods are provided for common use cases
 * like selecting entities from a container.
 *
 * @author Daniel Girardeau-Montaut
 *
 * @see ccEntitySelectionDlg
 */

#include <ui_itemSelectionDlg.h>

// qCC_db
#include <ccHObject.h>

/**
 * @brief Generic dialog for selecting items from a list.
 *
 * @details Provides a reusable UI for selecting items from a list.
 *
 * Features:
 * - Single or multi-selection mode
 * - Configurable item name and dialog label
 * - Static convenience methods for entity selection
 *
 * Usage:
 * @code
 * // Single selection
 * int index = ccItemSelectionDlg::SelectEntity(entities, 0, this);
 *
 * // Multi selection
 * std::vector<int> indexes;
 * if (ccItemSelectionDlg::SelectEntities(entities, indexes, this))
 * {
 *     // Process selected indexes
 * }
 * @endcode
 *
 * @extends QDialog
 * @extends Ui::ItemSelectionDlg
 */
class ccItemSelectionDlg : public QDialog
    , public Ui::ItemSelectionDlg
{
	Q_OBJECT

  public: // static shortcuts
	/**
	 * @brief Select a single entity from a container.
	 *
	 * @param[in] entities Available entities.
	 * @param[in] defaultSelectedIndex Index to select by default.
	 * @param[in] parent Parent widget.
	 * @param[in] label Dialog label text.
	 *
	 * @return Selected index, or -1 if cancelled.
	 */
	static int SelectEntity(const ccHObject::Container& entities,
	                        int                         defaultSelectedIndex = 0,
	                        QWidget*                    parent               = nullptr,
	                        QString                     label                = QString());

	/**
	 * @brief Select multiple entities from a container.
	 *
	 * @param[in] entities Available entities.
	 * @param[out] indexes Selected indexes.
	 * @param[in] parent Parent widget.
	 * @param[in] label Dialog label text.
	 *
	 * @return true if selection was made, false if cancelled.
	 */
	static bool SelectEntities(const ccHObject::Container& entities,
	                           std::vector<int>&           indexes,
	                           QWidget*                    parent = nullptr,
	                           QString                     label  = QString());

  public:
	/**
	 * @brief Construct the item selection dialog.
	 *
	 * @param[in] multiSelectionEnabled If true, allow multiple selections.
	 * @param[in] parent Parent widget.
	 * @param[in] itemName Name for items (e.g., "entities", "files").
	 * @param[in] label Dialog label text.
	 */
	ccItemSelectionDlg(bool     multiSelectionEnabled,
	                   QWidget* parent   = nullptr,
	                   QString  itemName = "entities",
	                   QString  label    = QString());

	/**
	 * @brief Set the list of items.
	 *
	 * @param[in] items List of item strings.
	 * @param[in] defaultSelectedIndex Index to select by default.
	 */
	void setItems(const QStringList& items, int defaultSelectedIndex = 0);

	/**
	 * @brief Get the selected index (single selection mode).
	 *
	 * @return Selected index, or -1 if none selected.
	 */
	int getSelectedIndex() const;

	/**
	 * @brief Get selected indexes (multi selection mode).
	 *
	 * @param[out] indexes Vector to fill with selected indexes.
	 */
	void getSelectedIndexes(std::vector<int>& indexes) const;
};

#endif // CC_ITEM_SELECTION_DIALOG_HEADER
