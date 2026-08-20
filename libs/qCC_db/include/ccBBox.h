// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccBBox.h
 *
 * @brief Axis-aligned bounding box (AABB) for spatial queries.
 *
 * @details Represents an axis-aligned bounding box for 3D entities.
 *
 * ## Overview
 *
 * Bounding boxes are used for:
 * - Spatial queries and culling
 * - Camera fitting
 * - Collision detection
 * - Octree construction
 *
 * ## Properties
 *
 * - **Min corner**: Smallest X, Y, Z values
 * - **Max corner**: Largest X, Y, Z values
 * - **Center**: Midpoint of box
 * - **Diagonal**: Corner-to-corner vector
 * - **Size**: Width, height, depth
 *
 * ## Usage
 *
 * @code
 * // Create bounding box
 * ccBBox box(minCorner, maxCorner, true);
 *
 * // Get properties
 * CCVector3 center = box.getCenter();
 * CCVector3d size = box.dimensions();
 *
 * // Transform
 * ccGLMatrix mat = ...;
 * ccBBox transformed = box * mat;
 *
 * // Check validity
 * if (box.isValid()) {
 *     // Use box
 * }
 *
 * // Render
 * box.draw(context, ccColor::red);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see CCCoreLib::BoundingBox for base implementation
 */

#ifndef CC_BBOX_HEADER
#define CC_BBOX_HEADER

// Local
#include "ccDrawableObject.h"

// CCCoreLib
#include <BoundingBox.h>

/**
 * @brief Axis-aligned bounding box (AABB).
 *
 * @details Extends CCCoreLib::BoundingBox with OpenGL rendering
 * and transformation capabilities.
 *
 * An AABB is defined by two corner points:
 * - Minimum: smallest X, Y, Z
 * - Maximum: largest X, Y, Z
 *
 * All edges are parallel to coordinate axes, hence "axis-aligned".
 *
 * @extends CCCoreLib::BoundingBox
 */
class QCC_DB_LIB_API ccBBox : public CCCoreLib::BoundingBox
{
  public:
	/**
	 * @brief Default constructor.
	 *
	 * Creates an invalid/empty bounding box.
	 */
	ccBBox()
	    : CCCoreLib::BoundingBox()
	{
	}

	/**
	 * @brief Construct from corner points.
	 *
	 * @param[in] bbMinCorner Minimum corner (x, y, z).
	 @param[in] bbMaxCorner Maximum corner (x, y, z).
	 * @param[in] valid Whether the box is valid.
	 */
	ccBBox(const CCVector3& bbMinCorner, const CCVector3& bbMaxCorner, bool valid)
	    : CCCoreLib::BoundingBox(bbMinCorner, bbMaxCorner, valid)
	{
	}

	/**
	 * @brief Copy constructor from CCCoreLib bounding box.
	 *
	 * @param[in] bbox Source bounding box.
	 */
	ccBBox(const CCCoreLib::BoundingBox& bbox)
	    : CCCoreLib::BoundingBox(bbox)
	{
	}

	/**
	 * @brief Apply a transformation matrix.
	 *
	 * @param[in] mat Transformation matrix.
	 *
	 * @return New transformed bounding box.
	 *
	 * @note The new box is not necessarily axis-aligned
	 *       if the transformation includes rotation.
	 */
	const ccBBox operator*(const ccGLMatrix& mat);

	/**
	 * @brief Apply a double-precision transformation matrix.
	 *
	 * @param[in] mat Transformation matrix.
	 *
	 * @return New transformed bounding box.
	 */
	const ccBBox operator*(const ccGLMatrixd& mat);

	/**
	 * @brief Render the bounding box (OpenGL).
	 *
	 * @param[in] context OpenGL drawing context.
	 * @param[in] col RGB color for rendering.
	 */
	void draw(CC_DRAW_CONTEXT& context, const ccColor::Rgb& col) const;
};

#endif // CC_BBOX_HEADER
