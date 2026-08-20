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
 * @brief Interpolation dialog
 *
 * Dialog for configuring interpolation parameters.
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ui_interpolationDlg.h>

// qCC_db
#include <ccPointCloudInterpolator.h>

/**
 * @brief Interpolation dialog
 *
 * Configure interpolation method and algorithm.
 */
class ccInterpolationDlg : public QDialog
    , public Ui::InterpolationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccInterpolationDlg(QWidget* parent = nullptr);

	/// Get interpolation method
	ccPointCloudInterpolator::Parameters::Method getInterpolationMethod() const;
	/// Set interpolation method
	void setInterpolationMethod(ccPointCloudInterpolator::Parameters::Method method);

	/// Get interpolation algorithm
	ccPointCloudInterpolator::Parameters::Algo getInterpolationAlgorithm() const;
	/// Set interpolation algorithm
	void setInterpolationAlgorithm(ccPointCloudInterpolator::Parameters::Algo algo);

	/// Set no normalization
	void setNoNormalization(bool state);
	/// Get no normalization
	bool noNormalization() const;

  protected:
	void onRadiusUpdated(double);
};
