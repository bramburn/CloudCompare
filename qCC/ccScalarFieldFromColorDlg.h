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

#ifndef CC_SF_FROM_COLOR_DLG_HEADER
#define CC_SF_FROM_COLOR_DLG_HEADER

/**
 * @file ccScalarFieldFromColorDlg.h
 *
 * @brief Scalar field from color dialog for extracting SFs from color channels.
 *
 * @details Dialog for extracting scalar fields from RGB(A) color channels.
 *
 * Converts color information to scalar fields for analysis:
 * - **R channel**: Red intensity as scalar field
 * - **G channel**: Green intensity as scalar field
 * - **B channel**: Blue intensity as scalar field
 * - **Alpha channel**: Transparency as scalar field
 * - **Composite**: Grayscale intensity (weighted average of RGB)
 *
 * Useful for:
 * - Analyzing intensity data encoded in colors
 * - Extracting individual color channels for processing
 * - Creating grayscale representations
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccScalarField
 */

#include <ui_scalarFieldFromColorDlg.h>

class ccPointCloud;

/**
 * @brief Dialog for extracting scalar fields from color channels.
 *
 * @details Provides a UI for choosing which color channels to
 * extract as scalar fields.
 *
 * @extends QDialog
 * @extends Ui::scalarFieldFromColorDlg
 */
class ccScalarFieldFromColorDlg : public QDialog
    , public Ui::scalarFieldFromColorDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccScalarFieldFromColorDlg(QWidget* parent = nullptr);

	/**
	 * @brief Check if R channel should be extracted.
	 * @return true if R channel is selected.
	 */
	bool getRStatus() const;

	/**
	 * @brief Check if G channel should be extracted.
	 * @return true if G channel is selected.
	 */
	bool getGStatus() const;

	/**
	 * @brief Check if B channel should be extracted.
	 * @return true if B channel is selected.
	 */
	bool getBStatus() const;

	/**
	 * @brief Check if Alpha channel should be extracted.
	 * @return true if Alpha channel is selected.
	 */
	bool getAlphaStatus() const;

	/**
	 * @brief Check if composite/grayscale should be extracted.
	 * @return true if composite is selected.
	 */
	bool getCompositeStatus() const;
};

#endif // CC_SF_FROM_COLOR_DLG_HEADER
