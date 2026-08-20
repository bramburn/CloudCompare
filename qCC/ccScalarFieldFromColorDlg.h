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
 * @brief Scalar field from color dialog
 *
 * Dialog for extracting scalar fields from color channels.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_scalarFieldFromColorDlg.h>

class ccPointCloud;

/**
 * @brief Scalar field from color dialog
 *
 * Extract scalar fields from color channels.
 */
class ccScalarFieldFromColorDlg : public QDialog
    , public Ui::scalarFieldFromColorDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccScalarFieldFromColorDlg(QWidget* parent = nullptr);

	/// Get R channel flag
	bool getRStatus() const;

	/// Get G channel flag
	bool getGStatus() const;

	/// Get B channel flag
	bool getBStatus() const;

	/// Get Alpha channel flag
	bool getAlphaStatus() const;

	/// Get Composite channel flag
	bool getCompositeStatus() const;
};

#endif // CC_SF_FROM_COLOR_DLG_HEADER
