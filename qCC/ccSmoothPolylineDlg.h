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
// #                                                                        //
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        //
// ##########################################################################

/**
 * @file ccSmoothPolylineDlg.h
 *
 * @brief Smooth polyline dialog for Chaikin smoothing.
 *
 * @details Dialog for configuring polyline smoothing using the Chaikin algorithm.
 *
 * Chaikin smoothing is a corner-cutting algorithm that produces smooth curves
 * by recursively cutting corners of a polyline. It:
 * - Preserves the overall shape of the polyline
 * - Creates visually smooth curves
 * - Does not require tangent information
 * - Works well for survey profiles and boundaries
 *
 * Parameters:
 * - **Iterations**: Number of smoothing passes (more = smoother)
 * - **Ratio**: Position of cut points (0.0-0.5, default 0.5)
 *
 * @author CloudCompare project
 */

#include <ccHObject.h>

// Qt
#include <QDialog>

class Ui_SmoothPolylineDialog;

/**
 * @brief Dialog for polyline smoothing using Chaikin algorithm.
 *
 * @details Provides a UI for configuring Chaikin smoothing parameters.
 *
 * The Chaikin algorithm works by:
 * 1. Cutting each corner at 1/4 and 3/4 positions
 * 2. Creating new points at these positions
 * 3. Repeating for desired iterations
 *
 * @extends QDialog
 */
class ccSmoothPolylineDialog : public QDialog
{
  public:
	/**
	 * @brief Construct the smooth polyline dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	ccSmoothPolylineDialog(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccSmoothPolylineDialog();

	/**
	 * @brief Set the iteration count.
	 *
	 * @param[in] count Number of smoothing iterations.
	 */
	void setIerationCount(int count);

	/**
	 * @brief Set the smoothing ratio.
	 *
	 * @param[in] ratio Cut ratio (0.0-0.5).
	 */
	void setRatio(double ratio);

	/**
	 * @brief Get the iteration count.
	 *
	 * @return Number of iterations.
	 */
	int getIerationCount() const;

	/**
	 * @brief Get the smoothing ratio.
	 *
	 * @return Cut ratio.
	 */
	double getRatio() const;

  protected:
	//! UI definition
	Ui_SmoothPolylineDialog* m_ui;
};
