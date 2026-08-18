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
 * @file ccCone.h
 *
 * @brief Cone primitive class
 *
 * Represents a 3D cone (or truncated cone / frustum) primitive.
 * The cone axis corresponds to the Z dimension.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccGenericPrimitive.h"

/**
 * @brief Cone primitive
 *
 * A 3D cone or truncated cone (frustum) with axis along Z.
 */
class QCC_DB_LIB_API ccCone : public ccGenericPrimitive
{
  public:
	//! Default drawing precision (angular steps)
	static const unsigned DEFAULT_DRAWING_PRECISION = 24;

	/**
	 * @brief Create a cone
	 * @param[in] bottomRadius Bottom radius
	 * @param[in] topRadius Top radius (0 = pointed tip)
	 * @param[in] height Cone height
	 * @param[in] xOff X-axis offset (snout mode)
	 * @param[in] yOff Y-axis offset (snout mode)
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Cone name
	 * @param[in] precision Drawing precision
	 * @param[in] uniqueID Optional unique ID
	 */
	ccCone(PointCoordinateType bottomRadius,
	       PointCoordinateType topRadius,
	       PointCoordinateType height,
	       PointCoordinateType xOff      = 0,
	       PointCoordinateType yOff      = 0,
	       const ccGLMatrix*   transMat  = nullptr,
	       QString             name      = QString("Cone"),
	       unsigned            precision = DEFAULT_DRAWING_PRECISION,
	       unsigned            uniqueID  = ccUniqueIDGenerator::InvalidUniqueID);

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccCone(QString name = QString("Cone"));

	//! Returns class ID
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::CONE;
	}

	//! Returns height
	inline PointCoordinateType getHeight() const
	{
		return m_height;
	}
	//! Sets height
	/** \warning changes primitive content (calls ccGenericPrimitive::updateRepresentation)
	 **/
	void setHeight(PointCoordinateType height);

	//! Returns bottom radius
	inline PointCoordinateType getBottomRadius() const
	{
		return m_bottomRadius;
	}
	//! Sets bottom radius
	/** \warning changes primitive content (calls ccGenericPrimitive::updateRepresentation)
	 **/
	virtual void setBottomRadius(PointCoordinateType radius);

	//! Returns top radius
	inline PointCoordinateType getTopRadius() const
	{
		return m_topRadius;
	}
	//! Sets top radius
	/** \warning changes primitive content (calls ccGenericPrimitive::updateRepresentation)
	 **/
	virtual void setTopRadius(PointCoordinateType radius);

	//! Returns cone axis bottom end point after applying transformation
	virtual CCVector3 getBottomCenter() const;
	//! Returns cone axis top end point after applying transformation
	virtual CCVector3 getTopCenter() const;

	//! Returns cone axis end point associated with whichever radii is smaller
	virtual CCVector3 getSmallCenter() const;
	//! Returns cone axis end point associated with whichever radii is larger
	virtual CCVector3 getLargeCenter() const;

	//! Returns whichever cone radii is smaller
	virtual PointCoordinateType getSmallRadius() const;
	//! Returns whichever cone radii is larger
	virtual PointCoordinateType getLargeRadius() const;

	//! Returns true if the Cone was created in snout mode
	virtual bool isSnoutMode() const
	{
		return (m_xOff != 0 || m_yOff != 0);
	}

	// inherited from ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Cone";
	}
	virtual bool hasDrawingPrecision() const override
	{
		return true;
	}
	virtual ccGenericPrimitive* clone() const override;

	//! Computes the cone apex position
	CCVector3 computeApex() const;

	//! Computes the cone half angle (in degrees)
	double computeHalfAngle_deg() const;

  protected:
	// inherited from ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	//! Bottom radius
	PointCoordinateType m_bottomRadius;

	//! Top radius
	PointCoordinateType m_topRadius;

	//! Displacement of axes along X-axis (Snout mode)
	PointCoordinateType m_xOff;

	//! Displacement of axes along Y-axis (Snout mode)
	PointCoordinateType m_yOff;

	//! Height
	PointCoordinateType m_height;
};
