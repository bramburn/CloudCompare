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
 * @brief GBL sensor projection dialog for ground-based lidar sensors.
 *
 * @details Dialog for configuring GBL (Ground-Based Lidar) sensor
 * projection parameters.
 *
 * GBL sensors are terrestrial laser scanners that capture 3D point clouds
 * from a fixed position. They typically provide:
 * - Spherical coordinates (range, azimuth, elevation)
 * - Intensity/reflectance values
 * - Multiple return information
 * - Timestamp data
 *
 * The projection dialog configures how to convert from the sensor's
 * native spherical coordinates to Cartesian 3D coordinates.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGBLSensor
 */

#include <ui_gblSensorProjectDlg.h>

class ccGBLSensor;

/**
 * @brief Dialog for GBL sensor projection parameters.
 *
 * @details Provides a UI for configuring ground-based lidar sensor
 * projection settings.
 *
 * Parameters typically include:
 * - Sensor position (X, Y, Z)
 * - Sensor orientation (heading, pitch, roll)
 * - Range limits (min/max distance)
 * - Scan resolution
 * - Output coordinate system
 *
 * @extends QDialog
 * @extends Ui::GBLSensorProjectDialog
 */
class ccGBLSensorProjectionDlg : public QDialog
    , public Ui::GBLSensorProjectDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the GBL sensor projection dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccGBLSensorProjectionDlg(QWidget* parent = nullptr);

	/**
	 * @brief Initialize with previous values.
	 *
	 * @details Loads saved values from the last session.
	 */
	void initWithPrevious();

	/**
	 * @brief Save values for next time.
	 *
	 * @details Persists current settings for future use.
	 */
	void saveForNextTime();

	/**
	 * @brief Initialize with an existing GBL sensor.
	 *
	 * @param[in] sensor GBL sensor to configure.
	 *
	 * @details Populates dialog fields from the sensor's parameters.
	 */
	void initWithGBLSensor(const ccGBLSensor* sensor);

	/**
	 * @brief Update a GBL sensor with dialog values.
	 *
	 * @param[in] sensor GBL sensor to update.
	 *
	 * @details Applies the configured parameters to the sensor.
	 */
	void updateGBLSensor(ccGBLSensor* sensor);
};
