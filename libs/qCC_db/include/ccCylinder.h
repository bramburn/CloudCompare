// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
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
 * @brief Cylinder primitive class.
 *
 * @details Represents a 3D cylinder primitive. Internally implemented as a
 * cone with equal top and bottom radii.
 *
 * ## Overview
 *
 * A cylinder is defined by:
 * - **Radius**: Circular cross-section radius
 * - **Height**: Length along the axis
 * - **Axis**: Along the Z dimension by default
 *
 * ## Implementation
 *
 * Cylinder is implemented as a special case of ccCone where
 * top radius equals bottom radius.
 *
 * ## Usage
 *
 * @code
 * // Create a cylinder
 * ccCylinder* cylinder = new ccCylinder(radius, height);
 *
 * // Set position and orientation
 * ccGLMatrix trans;
 * trans.initFromParameters(rotation, translation);
 * cylinder->setApplyTransformation(trans);
 *
 * // Clone the primitive
 * ccGenericPrimitive* clone = cylinder->clone();
 * @endcode
 *
 * @extends ccCone
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccCone for base implementation
 */

#pragma once

// Local
#include "ccCone.h"

/**
 * @brief Cylinder primitive.
 *
 * @details A 3D cylinder with circular cross-section.
 * Axis is along the Z dimension by default.
 *
 * @extends ccCone
 */
class QCC_DB_LIB_API ccCylinder : public ccCone
{
  public:
	//! Default drawing precision (angular steps).
	static const unsigned DEFAULT_DRAWING_PRECISION = 48;

	/**
	 * @brief Create a cylinder.
	 *
	 * @param[in] radius Cylinder radius.
	 * @param[in] height Cylinder height.
	 * @param[in] transMat Optional transformation matrix.
	 * @param[in] name Cylinder name.
	 * @param[in] precision Drawing precision (angular steps).
	 * @param[in] uniqueID Unique ID.
	 */
	ccCylinder(PointCoordinateType radius,
	           PointCoordinateType height,
	           const ccGLMatrix*   transMat  = nullptr,
	           QString             name      = QString("Cylinder"),
	           unsigned            precision = DEFAULT_DRAWING_PRECISION,
	           unsigned            uniqueID  = ccUniqueIDGenerator::InvalidUniqueID);

	/**
	 * @brief Simplified constructor.
	 *
	 * @note For ccHObject factory only!
	 *
	 * @param[in] name Cylinder name.
	 */
	ccCylinder(QString name = QString("Cylinder"));

	/**
	 * @brief Get class ID.
	 */
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::CYLINDER;
	}

	// inherited from ccGenericPrimitive
	/**
	 * @brief Get type name.
	 */
	virtual QString getTypeName() const override
	{
		return "Cylinder";
	}

	/**
	 * @brief Clone the primitive.
	 */
	virtual ccGenericPrimitive* clone() const override;

	// inherited from ccCone
	/**
	 * @brief Set bottom radius.
	 */
	virtual void setBottomRadius(PointCoordinateType radius) override;

	/**
	 * @brief Set top radius.
	 *
	 * @note For cylinder, top radius equals bottom radius.
	 */
	inline virtual void setTopRadius(PointCoordinateType radius) override
	{
		return setBottomRadius(radius);
	}
};
