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
 * @brief Extrude polyline dialog for creating meshes from polylines.
 *
 * @details Dialog for configuring parameters when extruding a 2D polyline
 * into a 3D mesh.
 *
 * Extrusion creates a mesh by extending the polyline in the perpendicular
 * direction (typically the Z axis):
 * - Height above: How far to extend upward from the polyline plane
 * - Depth below: How far to extend downward from the polyline plane
 *
 * This creates a "prism" or "wall" mesh from the polyline outline.
 *
 * @author CloudCompare project
 */

#include <ui_extrudePolylineDlg.h>

/**
 * @brief Dialog for polyline extrusion parameters.
 *
 * @details Provides a UI for setting the extrusion height
 * and depth when converting a 2D polyline to a 3D mesh.
 *
 * The dialog allows setting:
 * - Height above the polyline plane
 * - Depth below the polyline plane
 *
 * @extends QDialog
 * @extends Ui::ExtrudePolylineDialog
 */
class ccExtrudePolylineDlg : public QDialog
    , public Ui::ExtrudePolylineDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the extrusion dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccExtrudePolylineDlg(QWidget* parent = nullptr);

	/**
	 * @brief Get height above the polyline.
	 * @return Height value.
	 */
	double heightAbove() const;

	/**
	 * @brief Get depth below the polyline.
	 * @return Depth value.
	 */
	double depthBelow() const;

	/**
	 * @brief Set height above the polyline.
	 * @param[in] value Height value.
	 */
	void setHeightAbove(double value);

	/**
	 * @brief Set depth below the polyline.
	 * @param[in] value Depth value.
	 */
	void setDepthBelow(double value);
};

#endif // CC_EXTRUDE_POLYLINE_DLG_HEADER
