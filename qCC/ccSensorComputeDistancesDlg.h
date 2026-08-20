// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

#ifndef CC_SF_DISTANCES_DLG_HEADER
#define CC_SF_DISTANCES_DLG_HEADER

/**
 * @file ccSensorComputeDistancesDlg.h
 *
 * @brief Dialog for sensor range/distance computation parameters.
 *
 * @details Dialog for configuring how to compute distances from
 * sensor scan positions to points.
 *
 * Used with terrestrial laser scanners and similar sensors to:
 * - Calculate range values from sensor to each point
 * - Store distances as scalar fields
 * - Enable range-based filtering
 *
 * Options:
 * - **Squared distances**: Store d² instead of d (faster, saves memory)
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

namespace Ui
{
	class sensorComputeDistancesDlg;
}

/**
 * @brief Dialog for sensor distance computation settings.
 *
 * @details Provides options for how to compute and store
 * sensor-to-point distances.
 *
 * @extends QDialog
 */
class ccSensorComputeDistancesDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccSensorComputeDistancesDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccSensorComputeDistancesDlg();

	/**
	 * @brief Check if computing squared distances.
	 *
	 * @return true to store d², false for actual distance.
	 */
	bool computeSquaredDistances() const;

  private:
	//! UI definition
	Ui::sensorComputeDistancesDlg* m_ui;
};

#endif // CC_SF_DISTANCES_DLG_HEADER
