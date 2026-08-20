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
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

/**
 * @file ccFitSphereDlg.h
 *
 * @brief Fit sphere dialog
 *
 * Dialog for sphere fitting parameters.
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ui_fitShereDlg.h>

// Qt
#include <QDialog>

/**
 * @brief Fit sphere dialog
 *
 * Configure sphere fitting parameters.
 */
class ccFitSphereDlg : public QDialog
    , public Ui::FitSphereDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] maxOutliersRatio Max outliers ratio
	 * @param[in] confidence Confidence level
	 * @param[in] autoDetectSphereRadius Auto detect radius
	 * @param[in] sphereRadius Sphere radius
	 * @param[in] parent Parent widget
	 */
	ccFitSphereDlg(double   maxOutliersRatio,
	               double   confidence,
	               bool     autoDetectSphereRadius,
	               double   sphereRadius,
	               QWidget* parent = nullptr);

	/// Get max outliers ratio
	double maxOutliersRatio() const;
	/// Get confidence
	double confidence() const;
	/// Get auto detect sphere radius
	bool autoDetectSphereRadius() const;
	/// Get sphere radius
	double sphereRadius() const;
};
