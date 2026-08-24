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
 * @brief Entity selection dialog
 *
 * Dialog for selecting entities from a list.
 *
 * @author CloudCompare project
 */

// qCC_db
#include <ccHObject.h>

// Qt
#include <QDialog>

class Ui_EntitySelectionDialog;

/**
 * @brief Entity selection dialog
 *
 * Select one or multiple entities from a list.
 */
class ccEntitySelectionDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] entities Available entities
	 * @param[in] multiSelectionEnabled Allow multiple selection
	 * @param[in] defaultSelectedIndex Default selected index
	 * @param[in] parent Parent widget
	 * @param[in] label Dialog label
	 */
	ccEntitySelectionDialog(const ccHObject::Container& entities,
	                        bool multiSelectionEnabled,
	                        int defaultSelectedIndex = 0,
	                        QWidget* parent = nullptr,
	                        QString label = QString());

	/// Destructor
	virtual ~ccEntitySelectionDialog();

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
	/// Select all
	void selectAll();
	/// Select none
	void selectNone();

  protected:
	/// UI
	Ui_EntitySelectionDialog* m_ui;
};
