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
// #                                                                        #
// ##########################################################################

#include "ccCylinder.h"

// ccCylinder::ccCylinder
/**
 * @brief Construct a cylinder
 *
 * Delegates to ccCone with equal bottom and top radii (r = r, so a cylinder
 * is a frustum where both end caps have the same radius).
 *
 * @param[in] radius Circular cross-section radius
 * @param[in] height Z extent
 * @param[in] transMat Optional transformation matrix
 * @param[in] name Display name
 * @param[in] precision Angular tessellation steps
 * @param[in] uniqueID Optional unique ID
 */
ccCylinder::ccCylinder(PointCoordinateType radius,
                       PointCoordinateType height,
                       const ccGLMatrix* transMat /*=nullptr*/,
                       QString name /*=QString("Cylinder")*/,
                       unsigned precision /*=DEFAULT_DRAWING_PRECISION*/,
                       unsigned uniqueID /*=ccUniqueIDGenerator::InvalidUniqueID*/)
    : ccCone(radius, radius, height, 0, 0, transMat, name, precision, uniqueID)
{
}

// ccCylinder::ccCylinder
/**
 * @brief Simplified constructor for ccHObject factory
 *
 * @param[in] name Display name
 */
ccCylinder::ccCylinder(QString name /*=QString("Cylinder")*/)
    : ccCone(name)
{
}

// ccCylinder::clone
/**
 * @brief Clone this cylinder
 */
ccGenericPrimitive* ccCylinder::clone() const
{
	return finishCloneJob(new ccCylinder(m_bottomRadius, m_height, &m_transformation, getName(), m_drawPrecision));
}

// ccCylinder::setBottomRadius
/**
 * @brief Set the cylinder radius
 *
 * Overrides ccCone::setBottomRadius to enforce the cylinder invariant:
 * topRadius must always equal bottomRadius. Sets both simultaneously.
 *
 * @param[in] radius New radius (applied to both ends)
 */
void ccCylinder::setBottomRadius(PointCoordinateType radius)
{
	// Enforce the cylinder invariant: top radius must equal bottom radius
	m_topRadius = radius;
	ccCone::setBottomRadius(radius);
}
