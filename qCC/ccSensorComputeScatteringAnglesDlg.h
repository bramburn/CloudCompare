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

#ifndef CC_SF_SCATTERING_DLG_HEADER
#define CC_SF_SCATTERING_DLG_HEADER

/**
 * @file ccSensorComputeScatteringAnglesDlg.h
 *
 * @brief Sensor scattering angles dialog
 *
 * Dialog for scattering angles computation.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

namespace Ui
{
	class sensorComputeScatteringAnglesDlg;
}

/**
 * @brief Sensor scattering angles dialog
 *
 * Configure scattering angles computation.
 */
class ccSensorComputeScatteringAnglesDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccSensorComputeScatteringAnglesDlg(QWidget* parent = nullptr);

	/// Destructor
	~ccSensorComputeScatteringAnglesDlg();

	/// Get angles in degrees flag
	bool anglesInDegrees() const;

  private:
	Ui::sensorComputeScatteringAnglesDlg* m_ui;
};

#endif // CC_SF_SCATTERING_DLG_HEADER
