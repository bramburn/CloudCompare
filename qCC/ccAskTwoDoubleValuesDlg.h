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

#ifndef CC_ASK_TWO_DOUBLE_VALUES_DIALOG_HEADER
#define CC_ASK_TWO_DOUBLE_VALUES_DIALOG_HEADER

/**
 * @file ccAskTwoDoubleValuesDlg.h
 *
 * @brief Ask two values dialog
 *
 * Generic dialog for entering two double values.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_askTwoDoubleValuesDlg.h>

/**
 * @brief Ask two double values dialog
 *
 * Generic dialog for entering two double values with bounds.
 */
class ccAskTwoDoubleValuesDlg : public QDialog
    , public Ui::AskTwoDoubleValuesDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] vName1 Name of first value
	 * @param[in] vName2 Name of second value
	 * @param[in] minVal Minimum value
	 * @param[in] maxVal Maximum value
	 * @param[in] defaultVal1 Default value 1
	 * @param[in] defaultVal2 Default value 2
	 * @param[in] precision Decimal precision
	 * @param[in] windowTitle Window title
	 * @param[in] parent Parent widget
	 */
	ccAskTwoDoubleValuesDlg(const QString& vName1,
	                        const QString& vName2,
	                        double minVal,
	                        double maxVal,
	                        double defaultVal1,
	                        double defaultVal2,
	                        int precision = 6,
	                        QString windowTitle = QString(),
	                        QWidget* parent = nullptr);
};

#endif // CC_ASK_TWO_DOUBLE_VALUES_DIALOG_HEADER
