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
 * @brief Noise filter dialog
 *
 * Dialog for configuring noise filtering parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_noiseFilterDlg.h>

/**
 * @brief Noise filter dialog
 *
 * Configure noise filtering parameters.
 */
class ccNoiseFilterDlg : public QDialog
    , public Ui::NoiseFilterDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccNoiseFilterDlg(QWidget* parent = nullptr);
};
