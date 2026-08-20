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
 * @file ccAskThreeDoubleValuesDlg.h
 *
 * @brief Generic dialog for entering three double values.
 *
 * @details A reusable dialog for prompting the user to enter three
 * double values with configurable bounds and precision.
 *
 * Features:
 * - Three input fields with labels
 * - Configurable min/max range for all fields
 * - Configurable decimal precision
 * - Optional checkbox (e.g., "Apply to all" or "Use custom value")
 * - Optional window title override
 *
 * This is a utility dialog used throughout CloudCompare for
 * consistent value input when exactly three values are needed.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccAskTwoDoubleValuesDlg
 */

#include <ui_askThreeDoubleValuesDlg.h>

// Qt
#include <QString>

/**
 * @brief Generic dialog for entering three double values.
 *
 * @details A reusable dialog component that prompts for three
 * double values with consistent UI and validation.
 *
 * Use cases:
 * - Entering X, Y, Z coordinates
 * - Setting dimensions (width, height, depth)
 * - Configuring RGB values
 * - Any scenario requiring three numeric values
 *
 * The dialog validates input to ensure values are within
 * the specified range.
 *
 * @extends QDialog
 * @extends Ui::AskThreeDoubleValuesDialog
 */
class ccAskThreeDoubleValuesDlg : public QDialog
    , public Ui::AskThreeDoubleValuesDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the three-value dialog.
	 *
	 * @param[in] vName1 Label for the first value.
	 * @param[in] vName2 Label for the second value.
	 * @param[in] vName3 Label for the third value.
	 * @param[in] minVal Minimum allowed value for all fields.
	 * @param[in] maxVal Maximum allowed value for all fields.
	 * @param[in] defaultVal1 Default value for field 1.
	 * @param[in] defaultVal2 Default value for field 2.
	 * @param[in] defaultVal3 Default value for field 3.
	 * @param[in] precision Number of decimal places (default 6).
	 * @param[in] windowTitle Custom window title (uses default if empty).
	 * @param[in] parent Parent widget.
	 */
	ccAskThreeDoubleValuesDlg(const QString& vName1,
	                          const QString& vName2,
	                          const QString& vName3,
	                          double         minVal,
	                          double         maxVal,
	                          double         defaultVal1,
	                          double         defaultVal2,
	                          double         defaultVal3,
	                          int            precision   = 6,
	                          const QString  windowTitle = QString(),
	                          QWidget*       parent      = nullptr);

	/**
	 * @brief Show the optional checkbox.
	 *
	 * @param[in] label Checkbox label text.
	 * @param[in] state Initial checked state.
	 * @param[in] tooltip Tooltip text for the checkbox.
	 *
	 * @details Shows an optional checkbox below the value fields.
	 * The checkbox state can be queried with getCheckboxState().
	 * This is useful for "apply to all" or "use custom value" options.
	 */
	void showCheckbox(const QString& label, bool state, QString tooltip = QString());

	/**
	 * @brief Get the checkbox state.
	 *
	 * @return true if checkbox is checked.
	 *
	 * @note Only meaningful if showCheckbox() was called.
	 */
	bool getCheckboxState() const;
};
