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

#ifndef CC_MATCH_SCALES_DIALOG_HEADER
#define CC_MATCH_SCALES_DIALOG_HEADER

/**
 * @file ccMatchScalesDlg.h
 *
 * @brief Match scales dialog
 *
 * Dialog for scale matching algorithm selection.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_matchScalesDlg.h>

// Local
#include "ccLibAlgorithms.h"

/**
 * @brief Match scales dialog
 *
 * Select scale matching algorithm.
 */
class ccMatchScalesDlg : public QDialog
    , public Ui::MatchScalesDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] entities Available entities
	 * @param[in] defaultSelectedIndex Default selected index
	 * @param[in] parent Parent widget
	 */
	ccMatchScalesDlg(const ccHObject::Container& entities,
	                 int                         defaultSelectedIndex = 0,
	                 QWidget*                    parent               = nullptr);

	/// Get selected index
	int getSelectedIndex() const;

	/// Set selected algorithm
	void setSelectedAlgorithm(ccLibAlgorithms::ScaleMatchingAlgorithm algorithm);

	/// Get selected algorithm
	ccLibAlgorithms::ScaleMatchingAlgorithm getSelectedAlgorithm() const;
};

#endif // CC_ENTITY_PICKER_DIALOG_HEADER
