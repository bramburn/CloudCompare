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
 * @file ccAskTwoDoubleValuesDlg.cpp
 * @brief Implementation of two-value input dialog
 * @details A generic dialog for requesting two double values from the user.
 * @see ccAskTwoDoubleValuesDlg
 */

#include "ccAskTwoDoubleValuesDlg.h"

/**
 * @brief Constructor
 * @param vName1 Name/label for first value
 * @param vName2 Name/label for second value
 * @param minVal Minimum value for spin boxes
 * @param maxVal Maximum value for spin boxes
 * @param defaultVal1 Default value for first spin box
 * @param defaultVal2 Default value for second spin box
 * @param precision Decimal precision for spin boxes
 * @param windowTitle Optional window title
 * @param parent Parent widget
 */
ccAskTwoDoubleValuesDlg::ccAskTwoDoubleValuesDlg(const QString& vName1,
                                                 const QString& vName2,
                                                 double         minVal,
                                                 double         maxVal,
                                                 double         defaultVal1,
                                                 double         defaultVal2,
                                                 int            precision /*=6*/,
                                                 QString        windowTitle /*=QString()*/,
                                                 QWidget*       parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::AskTwoDoubleValuesDialog()
{
	setupUi(this);

	label1->setText(vName1);
	label2->setText(vName2);
	doubleSpinBox1->setDecimals(precision);
	doubleSpinBox2->setDecimals(precision);
	doubleSpinBox1->setRange(minVal, maxVal);
	doubleSpinBox2->setRange(minVal, maxVal);
	doubleSpinBox1->setValue(defaultVal1);
	doubleSpinBox2->setValue(defaultVal2);

	if (!windowTitle.isEmpty())
	{
		setWindowTitle(windowTitle);
	}
}
