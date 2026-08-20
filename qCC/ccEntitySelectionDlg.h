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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

/**
 * @file ccEntitySelectionDlg.h
 *
 * @brief Entity selection dialog for choosing entities from the database tree.
 *
 * @details Dialog for selecting one or more entities from the CloudCompare
 * database tree.
 *
 * Provides a UI for choosing entities with:
 * - Single or multi-selection mode
 * - Entity type filtering (optional)
 * - Visual preview of selected entities
 * - Search/filter functionality
 *
 * Static convenience methods allow quick entity picking without
 * explicitly creating a dialog instance.
 *
 * @author CloudCompare project
 *
 * @see ccHObject
 */

#include <ccHObject.h>

// Qt
#include <QDialog>

class Ui_EntitySelectionDialog;

/**
 * @brief Dialog for selecting entities from a list.
 *
 * @details Provides a UI for selecting entities from the database tree.
 *
 * Features:
 * - Single selection mode (returns one entity)
 * - Multi-selection mode (returns multiple entities)
 * - Select all/none buttons
 * - Entity type display with icons
 *
 * Usage:
 * @code
 * // Single selection
 * int index = ccEntitySelectionDialog::SelectEntity(entities, 0, this);
 *
 * // Multi selection
 * std::vector<int> indexes;
 * if (ccEntitySelectionDialog::SelectEntities(entities, indexes, this))
 * {
 *     // Process selected entities
 * }
 * @endcode
 *
 * @extends QDialog
 */
class ccEntitySelectionDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the entity selection dialog.
	 *
	 * @param[in] entities Available entities.
	 * @param[in] multiSelectionEnabled Allow multiple selections.
	 * @param[in] defaultSelectedIndex Index to select by default.
	 * @param[in] parent Parent widget.
	 * @param[in] label Dialog label text.
	 */
	ccEntitySelectionDialog(const ccHObject::Container& entities,
	                        bool                        multiSelectionEnabled,
	                        int                         defaultSelectedIndex = 0,
	                        QWidget*                    parent               = nullptr,
	                        QString                     label                = QString());

	/**
	 * @brief Destructor.
	 */
	virtual ~ccEntitySelectionDialog();

	/**
	 * @brief Get selected index (single selection).
	 * @return Selected index, or -1 if none.
	 */
	int getSelectedIndex() const;

	/**
	 * @brief Get selected indexes (multi selection).
	 * @param[out] indexes Selected indexes.
	 */
	void getSelectedIndexes(std::vector<int>& indexes) const;

	/**
	 * @brief Select a single entity (static convenience method).
	 *
	 * @param[in] entities Available entities.
	 * @param[in] defaultSelectedIndex Default selection.
	 * @param[in] parent Parent widget.
	 * @param[in] label Dialog label.
	 *
	 * @return Selected index, or -1 if cancelled.
	 */
	static int SelectEntity(const ccHObject::Container& entities,
	                        int                         defaultSelectedIndex = 0,
	                        QWidget*                    parent               = nullptr,
	                        QString                     label                = QString());

	/**
	 * @brief Select multiple entities (static convenience method).
	 *
	 * @param[in] entities Available entities.
	 * @param[out] indexes Selected indexes.
	 * @param[in] parent Parent widget.
	 * @param[in] label Dialog label.
	 *
	 * @return true if selection was made, false if cancelled.
	 */
	static bool SelectEntities(const ccHObject::Container& entities,
	                           std::vector<int>&           indexes,
	                           QWidget*                    parent = nullptr,
	                           QString                     label  = QString());

  public slots:
	/**
	 * @brief Select all entities.
	 */
	void selectAll();

	/**
	 * @brief Deselect all entities.
	 */
	void selectNone();

  protected:
	//! UI definition
	Ui_EntitySelectionDialog* m_ui;
};
