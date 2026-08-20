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
 * @brief Camera sensor projection dialog for configuring projection parameters.
 *
 * @details Dialog for setting up camera sensor projection parameters
 * when projecting 3D points onto a 2D image plane.
 *
 * Used with ccCameraSensor for:
 * - Calibrating camera parameters
 * - Setting intrinsic camera properties (focal length, principal point)
 * - Configuring distortion models
 * - Projecting point clouds onto camera images
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccCameraSensor
 * @see ccGBLSensor
 */

#include <ui_camSensorProjectDlg.h>

class ccCameraSensor;

/**
 * @brief Dialog for configuring camera sensor projection parameters.
 *
 * @details Provides a UI for setting up or modifying camera sensor
 * parameters used in perspective projection.
 *
 * Parameters typically include:
 * - Focal length (fx, fy)
 * - Principal point (cx, cy)
 * - Image dimensions
 * - Distortion coefficients
 * - Projection model (pinhole, fisheye, etc.)
 *
 * @extends QDialog
 * @extends Ui::CamSensorProjectDialog
 */
class ccCamSensorProjectionDlg : public QDialog
    , public Ui::CamSensorProjectDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the camera sensor projection dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccCamSensorProjectionDlg(QWidget* parent = nullptr);

	/**
	 * @brief Initialize dialog with an existing camera sensor.
	 *
	 * @param[in] sensor Camera sensor to configure.
	 *
	 * @details Populates the dialog fields with values from
	 * the given sensor for editing.
	 */
	void initWithCamSensor(const ccCameraSensor* sensor);

	/**
	 * @brief Update a camera sensor with dialog values.
	 *
	 * @param[in] sensor Camera sensor to update.
	 *
	 * @details Applies the parameters set in the dialog
	 * to the specified camera sensor.
	 */
	void updateCamSensor(ccCameraSensor* sensor);
};

#endif // CC_CAMERA_SENSOR_PROJECTION_DIALOG_HEADER
