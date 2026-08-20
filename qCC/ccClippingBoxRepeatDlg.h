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
 * @brief Clipping box repeat dialog
 *
 * Dialog for managing clipping box repeated processes.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_clippingBoxRepeatDlg.h>

// Qt
#include <QDialog>

/**
 * @brief Clipping box repeat dialog
 *
 * Configure clipping box for repeated processes.
 */
class ccClippingBoxRepeatDlg : public QDialog
    , public Ui::ClippingBoxRepeatDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] singleSliceMode Single slice mode
	 * @param[in] parent Parent widget
	 */
	ccClippingBoxRepeatDlg(bool singleSliceMode = false, QWidget* parent = nullptr);

	/**
	 * @brief Set flat dimension
	 * @param[in] dim Flat dimension (single slice mode)
	 */
	void setFlatDim(unsigned char dim);
	
	/**
	 * @brief Set repeat dimension
	 * @param[in] dim Repeat dimension (multi-slice mode)
	 */
	void setRepeatDim(unsigned char dim);

  protected:
	void onDimChecked(bool);
	void onDimXChecked(bool);
	void onDimYChecked(bool);
	void onDimZChecked(bool);
};

#endif // CC_CLIPPING_BOX_REPEAT_DIALOG_HEADER
