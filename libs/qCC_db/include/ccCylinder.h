// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccCylinder.h
 *
 * @brief Cylinder primitive for visualization and fitting
 *
 * Represents a finite 3D cylinder (right circular cylinder) positioned
 * in 3D space by the transformation matrix.
 *
 * The cylinder is implemented as a special case of ccCone where the
 * top and bottom radii are equal. The mesh consists of:
 * - Lateral surface: quadrilateral facets around the circumference
 * - Top and bottom caps: triangular fans or disks
 * - Normals: per-vertex for smooth shading
 *
 * Axis: along the local Z dimension by default. Use the transformation
 * matrix to reorient the axis.
 *
 * Drawing precision: 48 angular steps by default (configurable).
 * More steps = smoother appearance, more triangles.
 *
 * Use cases:
 * - Creating reference geometry (columns, poles)
 * - Fitting cylinders to point clouds
 * - Modeling cylindrical objects
 *
 * @extends ccCone
 */

#pragma once

#include "ccCone.h"

/**
 * @class ccCylinder
 *
 * @brief Right circular cylinder primitive
 *
 * A cylinder is a special case of a cone with equal top and bottom radii.
 * Axis is along local Z; rotation in the transformation matrix orients it.
 *
 * Mesh: quadrilateral lateral facets + top and bottom disks.
 *
 * @extends ccCone
 */
class QCC_DB_LIB_API ccCylinder : public ccCone
{
  public:
	//! Default angular tessellation steps (for smooth rendering)
	static const unsigned DEFAULT_DRAWING_PRECISION = 48;

	/**
	 * @brief Create a cylinder with specified dimensions
	 *
	 * @param[in] radius Circular cross-section radius
	 * @param[in] height Length along the Z axis
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Display name
	 * @param[in] precision Angular tessellation steps (default 48)
	 * @param[in] uniqueID Optional unique ID
	 */
	ccCylinder(PointCoordinateType radius,
	           PointCoordinateType height,
	           const ccGLMatrix* transMat = nullptr,
	           QString name = QString("Cylinder"),
	           unsigned precision = DEFAULT_DRAWING_PRECISION,
	           unsigned uniqueID = ccUniqueIDGenerator::InvalidUniqueID);

	/**
	 * @brief Simplified constructor for ccHObject factory
	 *
	 * @param[in] name Display name
	 */
	explicit ccCylinder(QString name = QString("Cylinder"));

	// ccHObject
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::CYLINDER;
	}

	// ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Cylinder";
	}

	/**
	 * @brief Clone this cylinder
	 */
	virtual ccGenericPrimitive* clone() const override;

	// ccCone

	/**
	 * @brief Set the cylinder radius
	 *
	 * Sets both bottom and top radii to the same value.
	 *
	 * @param[in] radius New radius
	 */
	virtual void setBottomRadius(PointCoordinateType radius) override;

	/**
	 * @brief Set the top radius
	 *
	 * For a cylinder, top and bottom radii are always equal.
	 * This is a no-op that redirects to setBottomRadius.
	 *
	 * @param[in] radius New radius (must equal bottom radius)
	 */
	inline virtual void setTopRadius(PointCoordinateType radius) override
	{
		setBottomRadius(radius);
	}
};
