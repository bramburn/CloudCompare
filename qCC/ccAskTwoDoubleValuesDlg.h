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
 * @brief Generic dialog for entering two double values.
 *
 * @details A reusable dialog for prompting the user to enter two
 * double values with configurable bounds and precision.
 *
 * Features:
 * - Two input fields with labels
 * - Configurable min/max range for both fields
 * - Configurable decimal precision
 * - Optional window title override
 *
 * Use cases:
 * - Entering min/max ranges
 * - Setting X, Y coordinates
 * - Configuring radius or diameter
 * - Any scenario requiring two numeric values
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccAskThreeDoubleValuesDlg
 */

#include <ui_askTwoDoubleValuesDlg.h>

/**
 * @brief Generic dialog for entering two double values.
 *
 * @details A reusable dialog component that prompts for two
 * double values with consistent UI and validation.
 *
 * The dialog validates input to ensure values are within
 * the specified range.
 *
 * @extends QDialog
 * @extends Ui::AskTwoDoubleValuesDialog
 */
class ccAskTwoDoubleValuesDlg : public QDialog
    , public Ui::AskTwoDoubleValuesDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the two-value dialog.
	 *
	 * @param[in] vName1 Label for the first value.
	 * @param[in] vName2 Label for the second value.
	 * @param[in] minVal Minimum allowed value for both fields.
	 * @param[in] maxVal Maximum allowed value for both fields.
	 * @param[in] defaultVal1 Default value for field 1.
	 * @param[in] defaultVal2 Default value for field 2.
	 * @param[in] precision Number of decimal places (default 6).
	 * @param[in] windowTitle Custom window title (uses default if empty).
	 * @param[in] parent Parent widget.
	 *
	 * @note Both fields share the same min/max range.
	 * For different ranges, a custom dialog should be used.
	 */
	ccAskTwoDoubleValuesDlg(const QString& vName1,
	                        const QString& vName2,
	                        double         minVal,
	                        double         maxVal,
	                        double         defaultVal1,
	                        double         defaultVal2,
	                        int            precision   = 6,
	                        QString        windowTitle = QString(),
	                        QWidget*       parent      = nullptr);
};

#endif // CC_ASK_TWO_DOUBLE_VALUES_DIALOG_HEADER
