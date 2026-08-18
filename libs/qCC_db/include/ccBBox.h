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

#ifndef CC_BBOX_HEADER
#define CC_BBOX_HEADER

/**
 * @file ccBBox.h
 *
 * @brief Bounding box class
 *
 * Axis-aligned bounding box (AABB) for spatial queries and rendering.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccDrawableObject.h"

// CCCoreLib
#include <BoundingBox.h>

/**
 * @brief Axis-aligned bounding box (AABB)
 *
 * Extends CCCoreLib::BoundingBox with OpenGL rendering
 * and transformation capabilities.
 */
class QCC_DB_LIB_API ccBBox : public CCCoreLib::BoundingBox
{
  public:
	/**
	 * @brief Default constructor
	 * Creates an invalid/empty bounding box.
	 */
	ccBBox()
	    : CCCoreLib::BoundingBox()
	{
	}
	/**
	 * @brief Constructor from corner points
	 * @param[in] bbMinCorner Minimum corner (x, y, z)
	 * @param[in] bbMaxCorner Maximum corner (x, y, z)
	 * @param[in] valid Whether the box is valid
	 */
	ccBBox(const CCVector3& bbMinCorner, const CCVector3& bbMaxCorner, bool valid)
	    : CCCoreLib::BoundingBox(bbMinCorner, bbMaxCorner, valid)
	{
	}
	/**
	 * @brief Copy constructor from CCCoreLib bounding box
	 * @param[in] bbox Source bounding box
	 */
	ccBBox(const CCCoreLib::BoundingBox& bbox)
	    : CCCoreLib::BoundingBox(bbox)
	{
	}

	/**
	 * @brief Apply a transformation matrix
	 * @param[in] mat Transformation matrix
	 * @return New bounding box
	 */
	const ccBBox operator*(const ccGLMatrix& mat);
	/**
	 * @brief Apply a double-precision transformation matrix
	 * @param[in] mat Transformation matrix
	 * @return New bounding box
	 */
	const ccBBox operator*(const ccGLMatrixd& mat);

	/**
	 * @brief Render the bounding box (OpenGL)
	 * @param[in] context OpenGL drawing context
	 * @param[in] col RGB color for rendering
	 */
	void draw(CC_DRAW_CONTEXT& context, const ccColor::Rgb& col) const;
};

#endif // CC_BBOX_HEADER
