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

#ifndef CC_EXPORT_COORD_TO_SF_DLG_HEADER
#define CC_EXPORT_COORD_TO_SF_DLG_HEADER

/**
 * @file ccExportCoordToSFDlg.h
 *
 * @brief Export coordinate to SF dialog
 *
 * Dialog for exporting coordinates to scalar fields.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_exportCoordToSFDlg.h>

/**
 * @brief Export coordinate to SF dialog
 *
 * Choose which coordinates to export as scalar fields.
 */
class ccExportCoordToSFDlg : public QDialog
    , public Ui::ExportCoordToSFDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccExportCoordToSFDlg(QWidget* parent = nullptr);

	/// Export X
	bool exportX() const;
	/// Export Y
	bool exportY() const;
	/// Export Z
	bool exportZ() const;
};

#endif // CC_EXPORT_COORD_TO_SF_DLG_HEADER
