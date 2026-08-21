// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_DISH_PRIMITIVE_HEADER
#define CC_DISH_PRIMITIVE_HEADER

/**
 * @file ccDish.h
 *
 * @brief Dish / spherical cap primitive
 *
 * Represents a dish shape: the curved surface of a sphere (or ellipsoid)
 * section, plus a flat bottom disk. Used for:
 * - Creating reference bowl/dish geometry
 * - Fitting dishes to point clouds
 * - Modeling domes and spherical caps
 *
 * Geometry:
 * - Base radius: the circular opening radius
 * - Height: depth of the dish from base to bottom
 * - Second radius (optional): ellipsoidal variant (if > 0)
 *
 * A dish is a spherical cap with radius r and height h. The curved surface
 * is a section of a sphere. The flat bottom is a disk.
 *
 * Variants:
 * - Spherical dish: m_secondRadius == 0 (circular cross-sections)
 * - Ellipsoidal dish: m_secondRadius > 0 (elliptical cross-sections)
 *
 * Drawing precision: 24 angular steps by default.
 *
 * @extends ccGenericPrimitive
 */

#include "ccGenericPrimitive.h"

/**
 * @class ccDish
 *
 * @brief Spherical/ellipsoidal dish primitive
 *
 * A dish is a curved surface plus flat bottom disk, forming a bowl shape.
 * The curved part is a section of a sphere (or ellipsoid) defined by
 * the base radius and height.
 *
 * @extends ccGenericPrimitive
 */
class QCC_DB_LIB_API ccDish : public ccGenericPrimitive
{
  public:
	//! Default angular tessellation steps
	static const unsigned DEFAULT_DRAWING_PRECISION = 24;

	/**
	 * @brief Create a dish
	 *
	 * @param[in] radius Base radius (the circular opening)
	 * @param[in] height Depth of the dish from base to bottom
	 * @param[in] radius2 Second radius for ellipsoidal variant (0 = spherical)
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Display name
	 * @param[in] precision Angular tessellation steps
	 */
	ccDish(PointCoordinateType radius,
	       PointCoordinateType height,
	       PointCoordinateType radius2   = 0,
	       const ccGLMatrix*   transMat  = nullptr,
	       QString             name      = QString("Dish"),
	       unsigned            precision = DEFAULT_DRAWING_PRECISION);

	/**
	 * @brief Simplified constructor for ccHObject factory
	 *
	 * @param[in] name Display name
	 */
	explicit ccDish(QString name = QString("Dish"));

	// ccHObject
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::DISH;
	}

	// ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Dish";
	}
	virtual bool hasDrawingPrecision() const override
	{
		return true;
	}
	virtual ccGenericPrimitive* clone() const override;

  protected:
	// ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	//! Circular opening radius
	PointCoordinateType m_baseRadius;
	//! Second radius for ellipsoidal shape (0 = spherical)
	PointCoordinateType m_secondRadius;
	//! Depth from base to bottom
	PointCoordinateType m_height;
};

#endif // CC_DISH_PRIMITIVE_HEADER
