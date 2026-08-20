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
// #                    COPYRIGHT: Daniel Girardeau-Montaut                 #
// #                                                                        #
// ##########################################################################

/**
 * @file ccInterpolationDlg.h
 *
 * @brief Interpolation dialog for configuring point cloud interpolation.
 *
 * @details Dialog for setting up interpolation parameters when
 * interpolating point cloud data onto a grid.
 *
 * Interpolation methods:
 * - Natural neighbor
 * - IDW (Inverse Distance Weighting)
 * - Kriging
 *
 * Used when rasterizing point clouds or filling gaps in data.
 *
 * @author Daniel Girardeau-Montaut
 *
 * @see ccPointCloudInterpolator
 */

#include <ui_interpolationDlg.h>

// qCC_db
#include <ccPointCloudInterpolator.h>

/**
 * @brief Dialog for configuring interpolation parameters.
 *
 * @details Provides a UI for setting up point cloud interpolation
 * parameters.
 *
 * Options:
 * - **Method**: The interpolation algorithm (natural neighbor, IDW, etc.)
 * - **Algorithm**: Specific algorithm parameters
 * - **Radius**: Search radius for interpolation
 * - **Normalization**: Whether to normalize results
 *
 * @extends QDialog
 * @extends Ui::InterpolationDlg
 */
class ccInterpolationDlg : public QDialog
    , public Ui::InterpolationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the interpolation dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccInterpolationDlg(QWidget* parent = nullptr);

	/**
	 * @brief Get the interpolation method.
	 *
	 * @return Interpolation method.
	 */
	ccPointCloudInterpolator::Parameters::Method getInterpolationMethod() const;

	/**
	 * @brief Set the interpolation method.
	 *
	 * @param[in] method Method to use.
	 */
	void setInterpolationMethod(ccPointCloudInterpolator::Parameters::Method method);

	/**
	 * @brief Get the interpolation algorithm.
	 *
	 * @return Interpolation algorithm.
	 */
	ccPointCloudInterpolator::Parameters::Algo getInterpolationAlgorithm() const;

	/**
	 * @brief Set the interpolation algorithm.
	 *
	 * @param[in] algo Algorithm to use.
	 */
	void setInterpolationAlgorithm(ccPointCloudInterpolator::Parameters::Algo algo);

	/**
	 * @brief Set no normalization mode.
	 *
	 * @param[in] state Enable/disable normalization.
	 */
	void setNoNormalization(bool state);

	/**
	 * @brief Check if normalization is disabled.
	 *
	 * @return true if normalization is disabled.
	 */
	bool noNormalization() const;

  protected slots:
	/**
	 * @brief Handle radius update.
	 *
	 * @param[in] value New radius value.
	 */
	void onRadiusUpdated(double value);
};
