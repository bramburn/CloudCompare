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

#ifndef CC_DISH_PRIMITIVE_HEADER
#define CC_DISH_PRIMITIVE_HEADER

/**
 * @file ccDish.h
 *
 * @brief Dish primitive
 *
 * 3D dish shape (section of sphere or ellipsoid).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccGenericPrimitive.h"

/**
 * @brief Dish primitive
 *
 * Section of sphere or half ellipsoid.
 */
class QCC_DB_LIB_API ccDish : public ccGenericPrimitive
{
  public:
	//! Default drawing precision
	static const unsigned DEFAULT_DRAWING_PRECISION = 24;

	/**
	 * @brief Create a dish
	 * @param[in] radius Base radius
	 * @param[in] height Height above base
	 * @param[in] radius2 0 = spherical, >0 = ellipsoidal
	 * @param[in] transMat Transformation matrix
	 * @param[in] name Dish name
	 * @param[in] precision Drawing precision
	 */
	ccDish(PointCoordinateType radius,
	       PointCoordinateType height,
	       PointCoordinateType radius2   = 0,
	       const ccGLMatrix*   transMat  = nullptr,
	       QString             name      = QString("Dish"),
	       unsigned            precision = DEFAULT_DRAWING_PRECISION);

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccDish(QString name = QString("Dish"));

	//! Returns class ID
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::DISH;
	}

	// inherited from ccGenericPrimitive
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
	// inherited from ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	//! Base radius
	PointCoordinateType m_baseRadius;
	//! Second radius
	PointCoordinateType m_secondRadius;
	//! Height
	PointCoordinateType m_height;
};

#endif // CC_DISH_PRIMITIVE_HEADER
