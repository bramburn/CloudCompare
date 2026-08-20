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

#ifndef CC_CLIPPING_BOX_REPEAT_DIALOG_HEADER
#define CC_CLIPPING_BOX_REPEAT_DIALOG_HEADER

/**
 * @file ccClippingBoxRepeatDlg.h
 *
 * @brief Clipping box repeat dialog for configuring repeated slicing.
 *
 * @details Dialog for configuring how the clipping box performs
 * repeated operations along one or more dimensions.
 *
 * Two modes:
 * - **Single slice mode**: Extract one slice from a flat plane
 * - **Repeat mode**: Extract multiple slices at intervals along an axis
 *
 * Used by ccClippingBoxTool for generating contour maps,
 * terrain profiles, or cross-sections.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccClippingBoxTool
 */

#include <ui_clippingBoxRepeatDlg.h>

// Qt
#include <QDialog>

/**
 * @brief Dialog for configuring repeated clipping box operations.
 *
 * @details Provides UI for setting up the direction and mode
 * for repeated clipping box slicing operations.
 *
 * Dimensions:
 * - 0 = X axis
 * - 1 = Y axis
 * - 2 = Z axis
 *
 * The dialog allows selecting which dimension is:
 * - The "flat" dimension (kept constant for single slice)
 * - The "repeat" dimension (stepped for multi-slice)
 *
 * @extends QDialog
 * @extends Ui::ClippingBoxRepeatDlg
 */
class ccClippingBoxRepeatDlg : public QDialog
    , public Ui::ClippingBoxRepeatDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the clipping box repeat dialog.
	 *
	 * @param[in] singleSliceMode If true, only single slice extraction.
	 * @param[in] parent Parent widget.
	 */
	ccClippingBoxRepeatDlg(bool singleSliceMode = false, QWidget* parent = nullptr);

	/**
	 * @brief Set the flat dimension.
	 *
	 * @param[in] dim Flat dimension (0=X, 1=Y, 2=Z).
	 *
	 * @details In single-slice mode, specifies which axis
	 * the slice is perpendicular to.
	 */
	void setFlatDim(unsigned char dim);

	/**
	 * @brief Set the repeat dimension.
	 *
	 * @param[in] dim Repeat dimension (0=X, 1=Y, 2=Z).
	 *
	 * @details In repeat mode, specifies which axis
	 * to step along when extracting multiple slices.
	 */
	void setRepeatDim(unsigned char dim);

  protected slots:
	/**
	 * @brief Handle dimension checkbox change.
	 * @param[in] checked Checked state.
	 */
	void onDimChecked(bool checked);

	/**
	 * @brief Handle X dimension checkbox.
	 * @param[in] checked Checked state.
	 */
	void onDimXChecked(bool checked);

	/**
	 * @brief Handle Y dimension checkbox.
	 * @param[in] checked Checked state.
	 */
	void onDimYChecked(bool checked);

	/**
	 * @brief Handle Z dimension checkbox.
	 * @param[in] checked Checked state.
	 */
	void onDimZChecked(bool checked);
};

#endif // CC_CLIPPING_BOX_REPEAT_DIALOG_HEADER
