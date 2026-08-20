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

#ifndef CC_COMPUTE_OCTREE_DLG_HEADER
#define CC_COMPUTE_OCTREE_DLG_HEADER

/**
 * @file ccComputeOctreeDlg.h
 *
 * @brief Compute octree dialog for configuring octree computation.
 *
 * @details Dialog for setting up octree computation parameters for
 * point clouds. Allows the user to specify:
 * - Computation mode (automatic or custom)
 * - Minimum cell size
 * - Custom bounding box
 *
 * The octree is a hierarchical spatial data structure that divides
 * 3D space recursively into octants. It's used for:
 * - Fast nearest neighbor queries
 * - Spatial indexing
 * - Level-of-detail rendering
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see DgmOctree
 * @see ccEntityAction::computeOctree()
 */

#include <ui_computeOctreeDlg.h>

// qCC_db
#include <ccBBox.h>

class ccBoundingBoxEditorDlg;

/**
 * @brief Dialog for configuring octree computation.
 *
 * @details Provides a UI for setting octree computation parameters.
 * The user can choose from three computation modes:
 *
 * 1. **DEFAULT**: Automatically determine cell size based on
 *    cloud characteristics.
 *
 * 2. **MIN_CELL_SIZE**: Specify a minimum cell size. The octree
 *    will subdivide cells down to this size.
 *
 * 3. **CUSTOM_BBOX**: Define a custom bounding box for the octree.
 *
 * @extends QDialog
 * @extends Ui::ComputeOctreeDialog
 */
class ccComputeOctreeDlg : public QDialog
    , public Ui::ComputeOctreeDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Computation modes for octree generation.
	 */
	enum ComputationMode
	{
		DEFAULT,       //!< Automatic cell size determination
		MIN_CELL_SIZE,  //!< User-specified minimum cell size
		CUSTOM_BBOX     //!< Custom bounding box
	};

	/**
	 * @brief Construct the octree computation dialog.
	 *
	 * @param[in] baseBBox Base bounding box for the cloud.
	 * @param[in] minCellSize Minimum allowed cell size.
	 * @param[in] parent Parent widget.
	 *
	 * @details Initializes the dialog with the cloud's bounding box
	 * and minimum cell size constraints.
	 */
	ccComputeOctreeDlg(const ccBBox& baseBBox,
	                   double        minCellSize,
	                   QWidget*      parent = nullptr);

	/**
	 * @brief Get the selected computation mode.
	 *
	 * @return The computation mode.
	 */
	ComputationMode getMode() const;

	/**
	 * @brief Get the minimum cell size.
	 *
	 * @return Cell size in world coordinates.
	 *
	 * @details Only meaningful when mode is MIN_CELL_SIZE.
	 */
	double getMinCellSize() const;

	/**
	 * @brief Get the custom bounding box.
	 *
	 * @return Custom bounding box.
	 *
	 * @details Only meaningful when mode is CUSTOM_BBOX.
	 */
	ccBBox getCustomBBox() const;

  protected:
	//! Bounding box editor for custom bbox mode
	ccBoundingBoxEditorDlg* m_bbEditorDlg;
};

#endif // CC_COMPUTE_OCTREE_DLG_HEADER
