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
 * @brief Match scales dialog for scale matching algorithm selection.
 *
 * @details Dialog for selecting the algorithm used to match the
 * scale/resolution of two point clouds.
 *
 * Scale matching is used before registration to normalize
 * the sampling density or resolution of clouds that may
 * have been captured at different scales.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccLibAlgorithms::ScaleMatchingAlgorithm
 */

#include <ui_matchScalesDlg.h>

// Local
#include "ccLibAlgorithms.h"

/**
 * @brief Dialog for selecting scale matching algorithm.
 *
 * @details Provides a UI for choosing the algorithm to use
 * when matching the scale of two point clouds.
 *
 * Scale matching algorithms can include:
 * - Subsampling to match lowest density
 * - Upsampling to match highest density
 * - Gaussian filtering
 * - Bilateral filtering
 *
 * @extends QDialog
 * @extends Ui::MatchScalesDialog
 */
class ccMatchScalesDlg : public QDialog
    , public Ui::MatchScalesDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the match scales dialog.
	 *
	 * @param[in] entities Available entities to match.
	 * @param[in] defaultSelectedIndex Default selection index.
	 * @param[in] parent Parent widget.
	 */
	ccMatchScalesDlg(const ccHObject::Container& entities,
	                 int                         defaultSelectedIndex = 0,
	                 QWidget*                    parent               = nullptr);

	/**
	 * @brief Get the selected entity index.
	 * @return Index of selected entity.
	 */
	int getSelectedIndex() const;

	/**
	 * @brief Set the selected algorithm.
	 * @param[in] algorithm Algorithm to select.
	 */
	void setSelectedAlgorithm(ccLibAlgorithms::ScaleMatchingAlgorithm algorithm);

	/**
	 * @brief Get the selected algorithm.
	 * @return Selected algorithm.
	 */
	ccLibAlgorithms::ScaleMatchingAlgorithm getSelectedAlgorithm() const;
};

#endif // CC_ENTITY_PICKER_DIALOG_HEADER
