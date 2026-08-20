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
 * @brief Labeling dialog for connected components labeling parameters.
 *
 * @details Dialog for configuring connected components labeling (also known
 * as blob labeling or cluster identification).
 *
 * Connected components labeling groups points that are spatially connected
 * into distinct clusters/objects. This is useful for:
 * - Segmentation of point clouds
 * - Object detection
 * - Statistical analysis of clusters
 *
 * Parameters:
 * - **Octree level**: Resolution for connectivity testing
 * - **Minimum points**: Minimum cluster size to keep
 * - **Random colors**: Assign random colors to each cluster
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEntityAction::labelConnectedComponents()
 */

#include <ui_labelingDlg.h>

/**
 * @brief Dialog for connected components labeling parameters.
 *
 * @details Provides a UI for setting up connected components
 * labeling parameters.
 *
 * The labeling algorithm:
 * 1. Uses octree structure for efficient neighbor queries
 * 2. Groups points that are connected (within threshold)
 * 3. Assigns a unique label to each connected component
 * 4. Optionally colors each component differently
 *
 * @extends QDialog
 * @extends Ui::LabelingDialog
 */
class ccLabelingDlg : public QDialog
    , public Ui::LabelingDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the labeling dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccLabelingDlg(QWidget* parent = nullptr);

	/**
	 * @brief Get the octree level.
	 * @return Octree subdivision level.
	 */
	int getOctreeLevel();

	/**
	 * @brief Get the minimum points per component.
	 * @return Minimum cluster size.
	 */
	int getMinPointsNb();

	/**
	 * @brief Check if random colors are enabled.
	 * @return true if random colors should be assigned.
	 */
	bool randomColors();

	/**
	 * @brief Set the octree level.
	 * @param[in] octreeLevel Level to set.
	 */
	void setOctreeLevel(int octreeLevel);

	/**
	 * @brief Set the minimum points per component.
	 * @param[in] minPointsNb Minimum cluster size.
	 */
	void setMinPointsNb(int minPointsNb);

	/**
	 * @brief Set random colors mode.
	 * @param[in] state Random colors state.
	 */
	void setRandomColors(bool state);
};
