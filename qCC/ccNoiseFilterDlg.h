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
 * @file ccNoiseFilterDlg.h
 *
 * @brief Noise filter dialog for configuring noise filtering parameters.
 *
 * @details Dialog for setting up parameters used by noise filtering
 * algorithms. Noise filtering removes outliers or noisy points
 * from point clouds based on statistical analysis.
 *
 * Typical noise filtering methods:
 * - Statistical outlier removal (SOR)
 * - Radius outlier removal
 * - Density-based filtering
 *
 * Parameters typically include:
 * - Number of neighbors to analyze
 * - Standard deviation multiplier (for statistical methods)
 * - Radius threshold (for density methods)
 * - Whether to filter isolated points
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEntityAction::filterNoise()
 */

#include <ui_noiseFilterDlg.h>

/**
 * @brief Dialog for configuring noise filtering parameters.
 *
 * @details Provides a UI for setting up noise filtering parameters
 * when removing outliers from point clouds.
 *
 * The dialog allows configuration of:
 * - Filter type (statistical, radius, etc.)
 * - Threshold parameters
 * - Processing options
 *
 * @extends QDialog
 * @extends Ui::NoiseFilterDialog
 */
class ccNoiseFilterDlg : public QDialog
    , public Ui::NoiseFilterDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the noise filter dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccNoiseFilterDlg(QWidget* parent = nullptr);
};
