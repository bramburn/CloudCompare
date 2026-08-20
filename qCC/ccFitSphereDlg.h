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
 * @brief Fit sphere dialog for sphere fitting parameters.
 *
 * @details Dialog for configuring parameters when fitting a sphere
 * to point cloud data.
 *
 * Sphere fitting is used to:
 * - Detect and measure spherical objects (targets, markers)
 * - Remove spherical noise/outliers
 * - Create primitive representations of round objects
 *
 * Parameters:
 * - **Max outliers ratio**: Maximum fraction of points that can
 *   be outliers while still considering the fit valid
 * - **Confidence**: Statistical confidence level for the fit
 * - **Auto detect radius**: Automatically determine sphere radius
 * - **Sphere radius**: Manual radius value (if not auto-detecting)
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ui_fitShereDlg.h>

// Qt
#include <QDialog>

/**
 * @brief Dialog for sphere fitting parameters.
 *
 * @details Provides a UI for setting up sphere fitting parameters.
 *
 * The dialog allows configuration of:
 * - Outlier tolerance
 * - Statistical confidence
 * - Radius detection mode
 *
 * @extends QDialog
 * @extends Ui::FitSphereDialog
 */
class ccFitSphereDlg : public QDialog
    , public Ui::FitSphereDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the sphere fitting dialog.
	 *
	 * @param[in] maxOutliersRatio Maximum outliers ratio.
	 * @param[in] confidence Statistical confidence level.
	 * @param[in] autoDetectSphereRadius Auto-detect radius.
	 * @param[in] sphereRadius Manual radius value.
	 * @param[in] parent Parent widget.
	 */
	ccFitSphereDlg(double   maxOutliersRatio,
	               double   confidence,
	               bool     autoDetectSphereRadius,
	               double   sphereRadius,
	               QWidget* parent = nullptr);

	/**
	 * @brief Get maximum outliers ratio.
	 * @return Maximum fraction of outliers allowed.
	 */
	double maxOutliersRatio() const;

	/**
	 * @brief Get confidence level.
	 * @return Confidence level (0-1).
	 */
	double confidence() const;

	/**
	 * @brief Check if radius is auto-detected.
	 * @return true if radius should be auto-detected.
	 */
	bool autoDetectSphereRadius() const;

	/**
	 * @brief Get sphere radius.
	 * @return Sphere radius value.
	 */
	double sphereRadius() const;
};
