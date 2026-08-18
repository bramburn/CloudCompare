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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccCylinder.h
 *
 * @brief Cylinder primitive class
 *
 * Represents a 3D cylinder primitive. Internally implemented as a
 * cone with equal top and bottom radii.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccCone.h"

/**
 * @brief Cylinder primitive
 *
 * A 3D cylinder. Axis is along the Z dimension.
 */
class QCC_DB_LIB_API ccCylinder : public ccCone
{
  public:
	//! Default drawing precision (angular steps)
	static const unsigned DEFAULT_DRAWING_PRECISION = 48;

	/**
	 * @brief Create a cylinder
	 * @param[in] radius Cylinder radius
	 * @param[in] height Cylinder height
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Cylinder name
	 * @param[in] precision Drawing precision
	 * @param[in] uniqueID Optional unique ID
	 */
	ccCylinder(PointCoordinateType radius,
	           PointCoordinateType height,
	           const ccGLMatrix*   transMat  = nullptr,
	           QString             name      = QString("Cylinder"),
	           unsigned            precision = DEFAULT_DRAWING_PRECISION,
	           unsigned            uniqueID  = ccUniqueIDGenerator::InvalidUniqueID);

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccCylinder(QString name = QString("Cylinder"));

	//! Returns class ID
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::CYLINDER;
	}

	// inherited from ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Cylinder";
	}
	virtual ccGenericPrimitive* clone() const override;

	// inherited from ccCone
	virtual void        setBottomRadius(PointCoordinateType radius) override;
	inline virtual void setTopRadius(PointCoordinateType radius) override
	{
		return setBottomRadius(radius);
	}
};
