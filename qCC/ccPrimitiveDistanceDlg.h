#pragma once

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
// #          COPYRIGHT: Chris Brown                                         #
// #                                                                        #
// ##########################################################################

/**
 * @file ccPrimitiveDistanceDlg.h
 *
 * @brief Primitive distance dialog for cloud-to-primitive distance settings.
 *
 * @details Dialog for configuring parameters when computing distances
 * from a point cloud to geometric primitives (plane, sphere, cylinder, etc.).
 *
 * Options:
 * - Signed distances (positive/negative based on which side of primitive)
 * - Flip normals (reverse distance sign convention)
 * - Treat planes as bounded (finite) vs. unbounded (infinite)
 *
 * @author Chris Brown
 */

#include <QDialog>
#include <ui_primitiveDistanceDlg.h>

/**
 * @brief Dialog for cloud-to-primitive distance settings.
 *
 * @details Provides a UI for setting up distance computation parameters
 * when comparing a point cloud to geometric primitives.
 *
 * Options:
 * - **Signed distances**: Return signed values based on which
 *   side of the primitive the point lies on
 * - **Flip normals**: Reverse the sign convention
 * - **Treat planes as bounded**: Consider planes as finite surfaces
 *   rather than infinite planes
 *
 * @extends QDialog
 * @extends Ui::primitiveDistanceDlg
 */
class ccPrimitiveDistanceDlg : public QDialog
    , public Ui::primitiveDistanceDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the primitive distance dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	ccPrimitiveDistanceDlg(QWidget* parent = nullptr);

	/**
	 * @brief Check if signed distances are enabled.
	 *
	 * @return true if signed distances should be computed.
	 *
	 * @details Signed distances distinguish between points
	 * inside/outside the primitive or above/below a plane.
	 */
	inline bool signedDistances() const
	{
		return signedDistCheckBox->isChecked();
	}

	/**
	 * @brief Check if normals should be flipped.
	 *
	 * @return true if normals should be reversed.
	 *
	 * @details Flipping normals reverses the sign
	 * convention for signed distances.
	 */
	inline bool flipNormals() const
	{
		return flipNormalsCheckBox->isChecked();
	}

	/**
	 * @brief Check if planes should be treated as bounded.
	 *
	 * @return true if planes are finite.
	 *
	 * @details When enabled, points beyond the plane's edges
	 * are considered outside. When disabled, planes extend
	 * infinitely.
	 */
	inline bool treatPlanesAsBounded() const
	{
		return treatPlanesAsBoundedCheckBox->isChecked();
	}
};
