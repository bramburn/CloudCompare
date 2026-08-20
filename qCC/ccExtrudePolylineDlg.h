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
// ##########################################################################

#ifndef CC_EXTRUDE_POLYLINE_DLG_HEADER
#define CC_EXTRUDE_POLYLINE_DLG_HEADER

/**
 * @file ccExtrudePolylineDlg.h
 *
 * @brief Extrude polyline dialog
 *
 * Dialog for extruding polylines into meshes.
 *
 * @author CloudCompare project
 */

#include <ui_extrudePolylineDlg.h>

/**
 * @brief Extrude polyline dialog
 *
 * Configure polyline extrusion parameters.
 */
class ccExtrudePolylineDlg : public QDialog
    , public Ui::ExtrudePolylineDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccExtrudePolylineDlg(QWidget* parent = nullptr);

	/// Get height above
	double heightAbove() const;
	/// Get depth below
	double depthBelow() const;

	/// Set height above
	void setHeightAbove(double value);
	/// Set depth below
	void setDepthBelow(double value);
};

#endif // CC_EXTRUDE_POLYLINE_DLG_HEADER
