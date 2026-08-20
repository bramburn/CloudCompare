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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_SELECT_CHILDREN_DLG_HEADER
#define CC_SELECT_CHILDREN_DLG_HEADER

/**
 * @file ccSelectChildrenDlg.h
 *
 * @brief Select children dialog
 *
 * Dialog for selecting children by type and name.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QDialog>

// qCC_db
#include <ccObject.h>

namespace Ui
{
	class SelectChildrenDialog;
}

/**
 * @brief Select children dialog
 *
 * Select children by type and name.
 */
class ccSelectChildrenDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccSelectChildrenDlg(QWidget* parent = nullptr);
	
	/// Destructor
	~ccSelectChildrenDlg() override;

	/**
	 * @brief Add type to combo box
	 * @param[in] typeName Type name
	 * @param[in] type Type enum
	 */
	void addType(QString typeName, CC_CLASS_ENUM type);

	/// Get selected type
	CC_CLASS_ENUM getSelectedType();
	
	/// Get selected name
	QString getSelectedName();
	
	/// Get strict match state
	bool getStrictMatchState() const;

	/// Get type is used flag
	bool getTypeIsUsed() const;

	/// Get name is regex flag
	bool getNameIsRegex() const;

	/// Get name match is used flag
	bool getNameMatchIsUsed() const;

  protected:
	/// Handle accept
	void onAccept();

  private:
	Ui::SelectChildrenDialog* mUI;
};

#endif // CC_SELECT_CHILDREN_DLG_HEADER
