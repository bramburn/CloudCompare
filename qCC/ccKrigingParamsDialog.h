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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

/**
 * @file ccKrigingParamsDialog.h
 *
 * @brief Kriging parameters dialog for configuring Kriging interpolation.
 *
 * @details Dialog for setting up parameters used by Kriging interpolation.
 *
 * Kriging is a geostatistical interpolation method that uses
 * spatial autocorrelation to estimate values at unmeasured locations.
 * It provides optimal (unbiased) estimates with minimum variance.
 *
 * Parameters typically include:
 * - Variogram model (spherical, exponential, Gaussian)
 * - Nugget (discontinuity at zero distance)
 * - Sill (total variance)
 * - Range (distance of spatial correlation)
 *
 * @author CloudCompare project
 *
 * @see ccRasterGrid::KrigingParams
 * @see ccInterpolationDlg
 */

// Qt
#include <QDialog>

// qCC_db
#include <ccRasterGrid.h>

class Ui_KrigingParamsDialog;

/**
 * @brief Dialog for configuring Kriging interpolation parameters.
 *
 * @details Provides a UI for setting up Kriging parameters
 * when using Kriging-based interpolation.
 *
 * Kriging parameters:
 * - **Model type**: Variogram model (linear, spherical, exponential, Gaussian)
 * - **Nugget**: Variance at zero distance (measurement error)
 * - **Sill**: Total variance (nugget + structured variance)
 * - **Range**: Distance beyond which correlation is zero
 *
 * @extends QDialog
 */
class ccKrigingParamsDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the Kriging parameters dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	ccKrigingParamsDialog(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccKrigingParamsDialog();

	/**
	 * @brief Set the Kriging parameters.
	 *
	 * @param[in] krigingParams Parameters to set.
	 */
	void setParameters(const ccRasterGrid::KrigingParams& krigingParams);

	/**
	 * @brief Get the current Kriging parameters.
	 *
	 * @param[out] krigingParams Current parameters.
	 */
	void getParameters(ccRasterGrid::KrigingParams& krigingParams);

  protected:
	//! UI definition
	Ui_KrigingParamsDialog* m_ui;
};
