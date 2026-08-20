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

#ifndef CC_BOUNDING_BOX_EDITOR_DLG_HEADER
#define CC_BOUNDING_BOX_EDITOR_DLG_HEADER

/**
 * @file ccBoundingBoxEditorDlg.h
 *
 * @brief Bounding box editor dialog for editing 3D bounding box extents.
 *
 * @details Interactive dialog for manually editing axis-aligned bounding
 * boxes. Used throughout CloudCompare for:
 * - Defining regions of interest
 * - Cropping entities
 * - Setting up raster grid extents
 * - Configuring clipping boxes
 *
 * Features:
 * - Numeric input for min/max coordinates
 * - "Square" mode to enforce equal X/Y dimensions
 * - 2D mode (lock one dimension)
 * - Base box constraints (box must be within base)
 * - Custom axis orientation
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccBBox
 * @see ccCropTool
 */

#include <ui_boundingBoxEditorDlg.h>

// qCC_db
#include <ccBBox.h>

/**
 * @brief Dialog for editing 3D bounding box dimensions.
 *
 * @details Provides an interactive UI for entering and modifying
 * axis-aligned bounding boxes. The dialog allows editing:
 * - Minimum and maximum X, Y, Z coordinates
 * - Box dimensions (width, height, depth)
 * - Box center position
 *
 * Features:
 * - **Square mode**: Enforces equal X and Y dimensions (useful for
 *   square tiles or voxels)
 * - **2D mode**: Locks one dimension (X, Y, or Z) for 2D editing
 * - **Base box constraint**: Ensures edited box stays within a
 *   base/parent bounding box
 * - **Custom axes**: Allows arbitrary X/Y/Z axis directions
 *
 * @extends QDialog
 * @extends Ui::BoundingBoxEditorDialog
 */
class ccBoundingBoxEditorDlg : public QDialog
    , public Ui::BoundingBoxEditorDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the bounding box editor.
	 *
	 * @param[in] showBoxAxes Whether to show axis orientation widgets.
	 * @param[in] showRasterGridImage Whether to show raster grid preview.
	 * @param[in] parent Parent widget.
	 */
	explicit ccBoundingBoxEditorDlg(bool showBoxAxes, bool showRasterGridImage, QWidget* parent = nullptr);

	/**
	 * @brief Get the current bounding box.
	 *
	 * @return Current bounding box.
	 */
	inline const ccBBox& getBox() const
	{
		return m_currentBBox;
	}

	/**
	 * @brief Set the bounding box.
	 *
	 * @param[in] box Bounding box to display.
	 */
	void setBox(const ccBBox& box);

	/**
	 * @brief Set the base bounding box.
	 *
	 * @param[in] box Base/parent bounding box.
	 * @param[in] isMinimal If true, edited box must be at least as large
	 *                     as the base box; if false, edited box must
	 *                     be contained within the base box.
	 */
	void setBaseBBox(const ccBBox& box, bool isMinimal = true);

	/**
	 * @brief Set custom box axes.
	 *
	 * @param[in] X Custom X axis direction.
	 * @param[in] Y Custom Y axis direction.
	 * @param[in] Z Custom Z axis direction.
	 *
	 * @details Allows the bounding box to use arbitrary axis directions
	 * instead of the default world axes.
	 */
	void setBoxAxes(const CCVector3& X, const CCVector3& Y, const CCVector3& Z);

	/**
	 * @brief Get the box axes.
	 *
	 * @param[out] X X axis direction.
	 * @param[out] Y Y axis direction.
	 * @param[out] Z Z axis direction.
	 */
	void getBoxAxes(CCVector3d& X, CCVector3d& Y, CCVector3d& Z);

	/**
	 * @brief Show inclusion warning.
	 *
	 * @param[in] state Whether to warn if box is outside base.
	 */
	void showInclusionWarning(bool state)
	{
		m_showInclusionWarning = state;
	}

	/**
	 * @brief Force square mode.
	 *
	 * @param[in] state Enable/disable square mode.
	 */
	void forceKeepSquare(bool state);

	/**
	 * @brief Check if square mode is active.
	 *
	 * @return true if square mode is enabled.
	 */
	bool keepSquare() const;

	/**
	 * @brief Set 2D editing mode.
	 *
	 * @param[in] state Enable/disable 2D mode.
	 * @param[in] dim Which dimension to lock (0=X, 1=Y, 2=Z).
	 */
	void set2DMode(bool state, unsigned char dim);

  public:
	/**
	 * @brief Execute the dialog.
	 *
	 * @return QDialog::Accepted or QDialog::Rejected.
	 */
	virtual int exec() override;

  protected slots:
	/**
	 * @brief Handle square mode activation.
	 *
	 * @param[in] state Square mode state.
	 */
	void squareModeActivated(bool state);

	/**
	 * @brief Reset to default box.
	 */
	void resetToDefault();

	/**
	 * @brief Reset to last saved box.
	 */
	void resetToLast();

	/**
	 * @brief Cancel and close.
	 */
	void cancel();

	/**
	 * @brief Save box and accept dialog.
	 */
	void saveBoxAndAccept();

	/**
	 * @brief Compute optimal dialog height.
	 *
	 * @param[in] showBoxAxes Whether box axes are shown.
	 * @param[in] showRasterGridImage Whether raster preview is shown.
	 * @return Optimal height in pixels.
	 */
	int computeBestDialogHeight(bool showBoxAxes, bool showRasterGridImage) const;

	/**
	 * @brief Handle X width change.
	 *
	 * @param[in] value New X width.
	 */
	void updateXWidth(double value);

	/**
	 * @brief Handle Y width change.
	 *
	 * @param[in] value New Y width.
	 */
	void updateYWidth(double value);

	/**
	 * @brief Handle Z width change.
	 *
	 * @param[in] value New Z width.
	 */
	void updateZWidth(double value);

	/**
	 * @brief Update current bounding box.
	 *
	 * @param[in] dummy Unused parameter.
	 */
	void updateCurrentBBox(double dummy = 0.0);

	/**
	 * @brief Reflect changes in UI.
	 *
	 * @param[in] dummy Unused parameter.
	 */
	void reflectChanges(int dummy = 0);

	/**
	 * @brief Handle axis value change.
	 *
	 * @param[in] value New axis value.
	 */
	void onAxisValueChanged(double value);

	/**
	 * @brief Load values from clipboard.
	 */
	void fromClipboardClicked();

	/**
	 * @brief Save values to clipboard.
	 */
	void toClipboardClicked();

  protected:
	/**
	 * @brief Check if box is within base box.
	 */
	void checkBaseInclusion();

	//! Base/parent bounding box
	ccBBox m_baseBBox;

	//! Whether base box is a minimum constraint
	bool m_baseBoxIsMinimal;

	//! Current bounding box
	ccBBox m_currentBBox;

	//! Initial bounding box
	ccBBox m_initBBox;

	//! Whether to show inclusion warning
	bool m_showInclusionWarning;
};

#endif // CC_BOUNDING_BOX_EDITOR_DLG_HEADER
