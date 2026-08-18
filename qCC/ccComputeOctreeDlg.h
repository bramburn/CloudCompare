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
 * @brief Compute octree dialog
 *
 * Dialog for configuring octree computation.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_computeOctreeDlg.h>

// qCC_db
#include <ccBBox.h>

class ccBoundingBoxEditorDlg;

/**
 * @brief Compute octree dialog
 *
 * Configure octree computation parameters.
 */
class ccComputeOctreeDlg : public QDialog
    , public Ui::ComputeOctreeDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] baseBBox Base bounding box
	 * @param[in] minCellSize Minimum cell size
	 * @param[in] parent Parent widget
	 */
	ccComputeOctreeDlg(const ccBBox& baseBBox,
	                   double        minCellSize,
	                   QWidget*      parent = nullptr);

	/// Computation mode
	enum ComputationMode
	{
		DEFAULT,      //!< Default mode
		MIN_CELL_SIZE, //!< Minimum cell size
		CUSTOM_BBOX    //!< Custom bbox
	};

	/**
	 * @brief Get computation mode
	 * @return Mode
	 */
	ComputationMode getMode() const;

	/**
	 * @brief Get minimum cell size
	 * @return Cell size
	 */
	double getMinCellSize() const;

	/**
	 * @brief Get custom bounding box
	 * @return Custom bbox
	 */
	ccBBox getCustomBBox() const;

  protected:
	/// Bounding box editor
	ccBoundingBoxEditorDlg* m_bbEditorDlg;
};

#endif // CC_COMPUTE_OCTREE_DLG_HEADER
