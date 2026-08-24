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
 * @file ccAskThreeDoubleValuesDlg.cpp
 * @brief Implementation of three-value input dialog
 * @details A generic dialog for requesting three double values from the user.
 * @see ccAskThreeDoubleValuesDlg
 */

#include "ccAskThreeDoubleValuesDlg.h"

// Qt
#include <QPushButton>

/**
 * @brief Constructor
 * @param vName1 Name/label for first value
 * @param vName2 Name/label for second value
 * @param vName3 Name/label for third value
 * @param minVal Minimum value for spin boxes
 * @param maxVal Maximum value for spin boxes
 * @param defaultVal1 Default value for first spin box
 * @param defaultVal2 Default value for second spin box
 * @param defaultVal3 Default value for third spin box
 * @param precision Decimal precision for spin boxes
 * @param windowTitle Optional window title
 * @param parent Parent widget
 */
ccAskThreeDoubleValuesDlg::ccAskThreeDoubleValuesDlg(const QString& vName1,
                                                     const QString& vName2,
                                                     const QString& vName3,
                                                     double minVal,
                                                     double maxVal,
                                                     double defaultVal1,
                                                     double defaultVal2,
                                                     double defaultVal3,
                                                     int precision /*=6*/,
                                                     const QString windowTitle /*=QString()*/,
                                                     QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::AskThreeDoubleValuesDialog()
{
	setupUi(this);

	checkBox->setVisible(false);

	label1->setText(vName1);
	label2->setText(vName2);
	label3->setText(vName3);
	doubleSpinBox1->setRange(minVal, maxVal);
	doubleSpinBox2->setRange(minVal, maxVal);
	doubleSpinBox3->setRange(minVal, maxVal);
	doubleSpinBox1->setValue(defaultVal1);
	doubleSpinBox2->setValue(defaultVal2);
	doubleSpinBox3->setValue(defaultVal3);
	doubleSpinBox1->setDecimals(precision);
	doubleSpinBox2->setDecimals(precision);
	doubleSpinBox3->setDecimals(precision);

	// automatically give the focus to the OK button
	if (buttonBox->button(QDialogButtonBox::Ok))
	{
		buttonBox->button(QDialogButtonBox::Ok)->setFocus();
	}

	if (!windowTitle.isEmpty())
	{
		setWindowTitle(windowTitle);
	}
}

/**
 * @brief Shows an optional checkbox
 * @param label Checkbox label text
 * @param state Initial checkbox state
 * @param tooltip Optional tooltip text
 * @details Makes the hidden checkbox visible for optional boolean input.
 */
void ccAskThreeDoubleValuesDlg::showCheckbox(const QString& label, bool state, QString tooltip /*=QString()*/)
{
	checkBox->setVisible(true);
	checkBox->setEnabled(true);
	checkBox->setChecked(state);
	checkBox->setText(label);
	checkBox->setToolTip(tooltip);
}

/**
 * @brief Gets the checkbox state
 * @return true if checkbox is checked
 */
bool ccAskThreeDoubleValuesDlg::getCheckboxState() const
{
	return checkBox->isChecked();
}
