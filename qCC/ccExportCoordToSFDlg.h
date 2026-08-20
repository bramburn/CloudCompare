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
 * @brief Export coordinate to scalar field dialog.
 *
 * @details Dialog for choosing which coordinates to export as
 * scalar fields when saving a point cloud.
 *
 * Allows exporting X, Y, and/or Z coordinates as separate
 * scalar fields, which can be useful for:
 * - Creating height maps
 * - Analyzing spatial distributions
 * - Colorizing based on position
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_exportCoordToSFDlg.h>

/**
 * @brief Dialog for choosing coordinates to export as scalar fields.
 *
 * @details Provides checkboxes for selecting which coordinate
 * axes (X, Y, Z) to export as scalar fields.
 *
 * @extends QDialog
 * @extends Ui::ExportCoordToSFDlg
 */
class ccExportCoordToSFDlg : public QDialog
    , public Ui::ExportCoordToSFDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the export dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccExportCoordToSFDlg(QWidget* parent = nullptr);

	/**
	 * @brief Check if X should be exported.
	 * @return true if X coordinate should be exported.
	 */
	bool exportX() const;

	/**
	 * @brief Check if Y should be exported.
	 * @return true if Y coordinate should be exported.
	 */
	bool exportY() const;

	/**
	 * @brief Check if Z should be exported.
	 * @return true if Z coordinate should be exported.
	 */
	bool exportZ() const;
};

#endif // CC_EXPORT_COORD_TO_SF_DLG_HEADER
