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
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_SELECT_CHILDREN_DLG_HEADER
#define CC_SELECT_CHILDREN_DLG_HEADER

/**
 * @file ccSelectChildrenDlg.h
 *
 * @brief Dialog for selecting children entities by type and name.
 *
 * @details Provides a dialog for filtering and selecting child entities
 * from a parent object in the database tree.
 *
 * Filtering options:
 * - Filter by entity type (cloud, mesh, polyline, etc.)
 * - Filter by name (with optional regex)
 * - Strict name matching
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

// qCC_db
#include <ccObject.h>

namespace Ui
{
	class SelectChildrenDialog;
}

/**
 * @brief Dialog for selecting children by type and name.
 *
 * @details Allows filtering children of a database tree entity.
 *
 * Features:
 * - Type-based filtering (point clouds, meshes, etc.)
 * - Name-based filtering with regex support
 * - Strict match mode for exact names
 *
 * @extends QDialog
 */
class ccSelectChildrenDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccSelectChildrenDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccSelectChildrenDlg() override;

	/**
	 * @brief Add a type option to the combo box.
	 *
	 * @param[in] typeName Display name for the type.
	 * @param[in] type CC_CLASS_ENUM value.
	 */
	void addType(QString typeName, CC_CLASS_ENUM type);

	/**
	 * @brief Get the selected entity type.
	 *
	 * @return Selected type, or CC_TYPES::UNKNOWN if none.
	 */
	CC_CLASS_ENUM getSelectedType();

	/**
	 * @brief Get the selected name filter.
	 *
	 * @return Name string.
	 */
	QString getSelectedName();

	/**
	 * @brief Check if strict name matching is enabled.
	 *
	 * @return true for exact match.
	 */
	bool getStrictMatchState() const;

	/**
	 * @brief Check if type filter is used.
	 *
	 * @return true if type filter is active.
	 */
	bool getTypeIsUsed() const;

	/**
	 * @brief Check if name is treated as regex.
	 *
	 * @return true if regex mode.
	 */
	bool getNameIsRegex() const;

	/**
	 * @brief Check if name matching is used.
	 *
	 * @return true if name filter is active.
	 */
	bool getNameMatchIsUsed() const;

  protected slots:
	/**
	 * @brief Handle dialog acceptance.
	 */
	void onAccept();

  private:
	//! UI definition
	Ui::SelectChildrenDialog* mUI;
};

#endif // CC_SELECT_CHILDREN_DLG_HEADER
