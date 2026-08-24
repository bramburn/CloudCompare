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
 * @file ccGBLSensorProjectionDlg.h
 *
 * @brief GBL sensor projection dialog
 *
 * Dialog for GBL (ground-based lidar) sensor parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_gblSensorProjectDlg.h>

class ccGBLSensor;

/**
 * @brief GBL sensor projection dialog
 *
 * Configure GBL sensor projection parameters.
 */
class ccGBLSensorProjectionDlg : public QDialog
    , public Ui::GBLSensorProjectDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccGBLSensorProjectionDlg(QWidget* parent = nullptr);

	/// Initialize with previous values
	void initWithPrevious();

	/// Save values for next time
	void saveForNextTime();

	/**
	 * @brief Initialize with GBL sensor
	 * @param[in] sensor GBL sensor
	 */
	void initWithGBLSensor(const ccGBLSensor* sensor);

	/**
	 * @brief Update GBL sensor
	 * @param[in] sensor GBL sensor
	 */
	void updateGBLSensor(ccGBLSensor* sensor);
};
