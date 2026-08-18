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

#ifndef CC_SPHERE_PRIMITIVE_HEADER
#define CC_SPHERE_PRIMITIVE_HEADER

/**
 * @file ccSphere.h
 *
 * @brief Sphere primitive class
 *
 * Represents a 3D sphere primitive for visualization and fitting.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccGenericPrimitive.h"

/**
 * @brief Sphere primitive
 *
 * A 3D sphere for visualization and geometric fitting.
 */
class QCC_DB_LIB_API ccSphere : public ccGenericPrimitive
{
  public:
	/**
	 * @brief Create a sphere
	 * @param[in] radius Sphere radius
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Sphere name
	 * @param[in] precision Drawing precision (angular steps)
	 * @param[in] uniqueID Optional unique ID
	 */
	ccSphere(PointCoordinateType radius,
	         const ccGLMatrix*   transMat  = nullptr,
	         QString             name      = QString("Sphere"),
	         unsigned            precision = 24,
	         unsigned            uniqueID  = ccUniqueIDGenerator::InvalidUniqueID);

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccSphere(QString name = QString("Sphere"));

	//! Returns class ID
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::SPHERE;
	}

	// inherited from ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Sphere";
	}
	virtual bool hasDrawingPrecision() const override
	{
		return true;
	}
	virtual ccGenericPrimitive* clone() const override;

	//! Returns radius
	inline PointCoordinateType getRadius() const
	{
		return m_radius;
	}
	//! Sets radius
	/** \warning changes primitive content (calls ccGenericPrimitive::updateRepresentation)
	 **/
	void setRadius(PointCoordinateType radius);

  protected:
	// inherited from ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	// inherited from ccHObject
	virtual void drawNameIn3D(CC_DRAW_CONTEXT& context) override;

	//! Radius
	PointCoordinateType m_radius;
};

#endif // CC_SPHERE_PRIMITIVE_HEADER
