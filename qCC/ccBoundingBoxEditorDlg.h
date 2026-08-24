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
 * @brief Bounding box editor dialog
 *
 * Dialog for editing 3D bounding box extents.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_boundingBoxEditorDlg.h>

// qCC_db
#include <ccBBox.h>

/**
 * @brief Bounding box editor dialog
 *
 * Dialog for editing 3D bounding box dimensions.
 */
class ccBoundingBoxEditorDlg : public QDialog
    , public Ui::BoundingBoxEditorDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] showBoxAxes Show box axes
	 * @param[in] showRasterGridImage Show raster grid image
	 * @param[in] parent Parent widget
	 */
	explicit ccBoundingBoxEditorDlg(bool showBoxAxes, bool showRasterGridImage, QWidget* parent = nullptr);

	/**
	 * @brief Get bounding box
	 * @return Current bounding box
	 */
	inline const ccBBox& getBox() const
	{
		return m_currentBBox;
	}

	/**
	 * @brief Set bounding box
	 * @param[in] box Bounding box
	 */
	void setBox(const ccBBox& box);

	/**
	 * @brief Set base box
	 * @param[in] box Base box
	 * @param[in] isMinimal Require box to be at least this large
	 */
	void setBaseBBox(const ccBBox& box, bool isMinimal = true);

	/// Set box axes
	void setBoxAxes(const CCVector3& X, const CCVector3& Y, const CCVector3& Z);

	/// Get box axes
	void getBoxAxes(CCVector3d& X, CCVector3d& Y, CCVector3d& Z);

	/**
	 * @brief Show inclusion warning
	 * @param[in] state Show state
	 */
	void showInclusionWarning(bool state)
	{
		m_showInclusionWarning = state;
	}

	/// Force keep square mode
	void forceKeepSquare(bool state);

	/// Check keep square mode
	bool keepSquare() const;

	/**
	 * @brief Set 2D mode
	 * @param[in] state 2D mode
	 * @param[in] dim Hidden dimension
	 */
	void set2DMode(bool state, unsigned char dim);

  public:
	// overloaded from QDialog
	virtual int exec();

  protected:
	void squareModeActivated(bool);
	void resetToDefault();
	void resetToLast();
	void cancel();
	void saveBoxAndAccept();
	int computeBestDialogHeight(bool showBoxAxes, bool showRasterGridImage) const;

	void updateXWidth(double);
	void updateYWidth(double);
	void updateZWidth(double);

	void updateCurrentBBox(double dummy = 0.0);
	void reflectChanges(int dummy = 0);

	void onAxisValueChanged(double);

	void fromClipboardClicked();
	void toClipboardClicked();

  protected:
	void checkBaseInclusion();

	ccBBox m_baseBBox;
	bool m_baseBoxIsMinimal;
	ccBBox m_currentBBox;
	ccBBox m_initBBox;
	bool m_showInclusionWarning;
};

#endif // CC_BOUNDING_BOX_EDITOR_DLG_HEADER
