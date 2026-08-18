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
 * @brief Kriging parameters dialog
 *
 * Dialog for configuring Kriging interpolation parameters.
 *
 * @author CloudCompare project
 */

// Qt
#include <QDialog>

// qCC_db
#include <ccRasterGrid.h>

class Ui_KrigingParamsDialog;

/**
 * @brief Kriging parameters dialog
 *
 * Configure Kriging interpolation parameters.
 */
class ccKrigingParamsDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	ccKrigingParamsDialog(QWidget* parent = nullptr);

	/// Destructor
	virtual ~ccKrigingParamsDialog();

	/**
	 * @brief Set parameters
	 * @param[in] krigingParams Parameters
	 */
	void setParameters(const ccRasterGrid::KrigingParams& krigingParams);

	/**
	 * @brief Get parameters
	 * @param[out] krigingParams Parameters
	 */
	void getParameters(ccRasterGrid::KrigingParams& krigingParams);

  protected:
	/// UI
	Ui_KrigingParamsDialog* m_ui;
};
