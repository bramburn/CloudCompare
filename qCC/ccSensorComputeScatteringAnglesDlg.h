// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        //
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

#ifndef CC_SF_SCATTERING_DLG_HEADER
#define CC_SF_SCATTERING_DLG_HEADER

/**
 * @file ccSensorComputeScatteringAnglesDlg.h
 *
 * @brief Dialog for scattering angles computation from sensor data.
 *
 * @details Dialog for configuring the computation of scattering angles
 * from terrestrial laser scanner data.
 *
 * Scattering angles are calculated based on:
 * - Sensor position
 * - Point position
 * - Laser beam direction
 *
 * Used for:
 * - Reflectance analysis
 * - Incidence angle correction
 * - Surface property estimation
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

namespace Ui
{
	class sensorComputeScatteringAnglesDlg;
}

/**
 * @brief Dialog for scattering angles computation settings.
 *
 * @details Provides options for how to compute and display
 * scattering/incidence angles.
 *
 * @extends QDialog
 */
class ccSensorComputeScatteringAnglesDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccSensorComputeScatteringAnglesDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccSensorComputeScatteringAnglesDlg();

	/**
	 * @brief Check if angles should be in degrees.
	 *
	 * @return true for degrees, false for radians.
	 */
	bool anglesInDegrees() const;

  private:
	//! UI definition
	Ui::sensorComputeScatteringAnglesDlg* m_ui;
};

#endif // CC_SF_SCATTERING_DLG_HEADER
