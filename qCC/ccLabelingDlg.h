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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccLabelingDlg.h
 *
 * @brief Labeling dialog
 *
 * Dialog for connected components labeling parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_labelingDlg.h>

/**
 * @brief Labeling dialog
 *
 * Configure connected components labeling parameters.
 */
class ccLabelingDlg : public QDialog
    , public Ui::LabelingDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccLabelingDlg(QWidget* parent = nullptr);

	/// Get octree level
	int getOctreeLevel();

	/// Get min points per component
	int getMinPointsNb();

	/// Get random colors flag
	bool randomColors();

	/// Set octree level
	void setOctreeLevel(int octreeLevel);

	/// Set min points per component
	void setMinPointsNb(int minPointsNb);

	/// Set random colors flag
	void setRandomColors(bool state);
};
