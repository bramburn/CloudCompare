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
 * @file ccSmoothPolylineDlg.h
 *
 * @brief Smooth polyline dialog
 *
 * Dialog for smoothing polylines using Chaikin algorithm.
 *
 * @author CloudCompare project
 */

// qCC_db
#include <ccHObject.h>

// Qt
#include <QDialog>

class Ui_SmoothPolylineDialog;

/**
 * @brief Smooth polyline dialog
 *
 * Smooth polylines using Chaikin algorithm.
 */
class ccSmoothPolylineDialog : public QDialog
{
  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	ccSmoothPolylineDialog(QWidget* parent = nullptr);

	/// Destructor
	virtual ~ccSmoothPolylineDialog();

	/// Set iteration count
	void setIerationCount(int count);

	/// Set smoothing ratio
	void setRatio(double ratio);

	/// Get iteration count
	int getIerationCount() const;

	/// Get smoothing ratio
	double getRatio() const;

  protected:
	/// UI
	Ui_SmoothPolylineDialog* m_ui;
};
