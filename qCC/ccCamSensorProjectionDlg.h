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

#ifndef CC_CAMERA_SENSOR_PROJECTION_DIALOG_HEADER
#define CC_CAMERA_SENSOR_PROJECTION_DIALOG_HEADER

/**
 * @file ccCamSensorProjectionDlg.h
 *
 * @brief Camera sensor projection dialog
 *
 * Dialog for camera sensor projection parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_camSensorProjectDlg.h>

class ccCameraSensor;

/**
 * @brief Camera sensor projection dialog
 *
 * Configure camera sensor projection parameters.
 */
class ccCamSensorProjectionDlg : public QDialog
    , public Ui::CamSensorProjectDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccCamSensorProjectionDlg(QWidget* parent = nullptr);

	/**
	 * @brief Initialize with camera sensor
	 * @param[in] sensor Camera sensor
	 */
	void initWithCamSensor(const ccCameraSensor* sensor);

	/**
	 * @brief Update camera sensor
	 * @param[in] sensor Camera sensor
	 */
	void updateCamSensor(ccCameraSensor* sensor);
};

#endif // CC_CAMERA_SENSOR_PROJECTION_DIALOG_HEADER
