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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccPickOneElementDlg.h
 *
 * @brief Single-item selection dialog
 *
 * A simple modal dialog presenting a QComboBox (drop-down list)
 * for selecting exactly one item from a list. Used throughout
 * CloudCompare for choosing from a finite set of options when
 * a more complex dialog is not warranted.
 *
 * Usage:
 * @code
 * ccPickOneElementDlg dlg("Choose method:", "Method", this);
 * dlg.addElement("Method A");
 * dlg.addElement("Method B");
 * dlg.addElement("Method C");
 * dlg.setDefaultIndex(1);
 * if (dlg.exec() == QDialog::Accepted) {
 *     int idx = dlg.getSelectedIndex();
 *     // ...
 * }
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "CCAppCommon.h"

// Qt
#include <QDialog>

class Ui_PickOneElementDialog;

/**
 * @brief Pick one element dialog
 *
 * Simple dialog for selecting one item from a list.
 */
class CCAPPCOMMON_LIB_API ccPickOneElementDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 *
	 * @param[in] label Label text displayed above the combobox
	 * @param[in] windowTitle Window title (default = empty)
	 * @param[in] parent Parent widget
	 */
	ccPickOneElementDlg(const QString& label,
	                    const QString& windowTitle = QString(),
	                    QWidget* parent = nullptr);

	/**
	 * @brief Destructor
	 */
	~ccPickOneElementDlg() override;

	/**
	 * @brief Add element to list
	 * @param[in] elementName Element name
	 */
	void addElement(const QString& elementName);

	/**
	 * @brief Set default selection
	 * @param[in] index Default index
	 */
	void setDefaultIndex(int index);

	/**
	 * @brief Get selected index
	 * @return Selected index
	 */
	int getSelectedIndex();

  private:
	/// UI
	Ui_PickOneElementDialog* m_ui;
};
