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
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccCone.h
 *
 * @brief Cone and frustum primitive
 *
 * Represents a 3D cone or truncated cone (frustum) with axis along Z.
 *
 * Geometry:
 * - Bottom radius: circular cross-section at Z=0
 * - Top radius: circular cross-section at Z=height (can be 0 for pointed tip)
 * - Axis: along local Z dimension
 * - Snout mode: optional X/Y offset of the top center
 *
 * Mesh:
 * - Lateral surface: triangular facets around the circumference
 * - Top cap: disk (if topRadius > 0) or single point (if topRadius == 0)
 * - Bottom cap: disk
 * - Normals: per-vertex for smooth shading
 *
 * Special cases:
 * - Cylinder: topRadius == bottomRadius (use ccCylinder)
 * - Pointed cone: topRadius == 0
 * - Frustum: both radii > 0 and unequal
 *
 * Snout mode (m_xOff/m_yOff != 0): the top disk center is displaced
 * from the axis, creating an offset "snout" shape (used for some
 * primitives like ccDish).
 *
 * Drawing precision: 24 angular steps by default.
 *
 * @extends ccGenericPrimitive
 */

#include "ccGenericPrimitive.h"

/**
 * @class ccCone
 *
 * @brief 3D cone / frustum / truncated cone primitive
 *
 * A cone is defined by:
 * - Bottom radius (at Z=0)
 * - Top radius (at Z=height), can be 0 for a pointed tip
 * - Height (Z extent)
 * - Optional snout offset (m_xOff, m_yOff)
 * - Transformation matrix (position + orientation)
 *
 * Mesh: quadrilateral/triangular facets with smooth normals.
 *
 * @extends ccGenericPrimitive
 */
class QCC_DB_LIB_API ccCone : public ccGenericPrimitive
{
  public:
	//! Default angular tessellation steps
	static const unsigned DEFAULT_DRAWING_PRECISION = 24;

	/**
	 * @brief Create a cone or frustum
	 *
	 * @param[in] bottomRadius Radius at Z=0 (must be > 0)
	 * @param[in] topRadius Radius at Z=height (can be 0 for pointed tip)
	 * @param[in] height Z extent
	 * @param[in] xOff X-axis offset for snout mode (default 0)
	 * @param[in] yOff Y-axis offset for snout mode (default 0)
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Display name
	 * @param[in] precision Angular tessellation steps
	 * @param[in] uniqueID Optional unique ID
	 */
	ccCone(PointCoordinateType bottomRadius,
	       PointCoordinateType topRadius,
	       PointCoordinateType height,
	       PointCoordinateType xOff = 0,
	       PointCoordinateType yOff = 0,
	       const ccGLMatrix* transMat = nullptr,
	       QString name = QString("Cone"),
	       unsigned precision = DEFAULT_DRAWING_PRECISION,
	       unsigned uniqueID = ccUniqueIDGenerator::InvalidUniqueID);

	/**
	 * @brief Simplified constructor for ccHObject factory
	 *
	 * @param[in] name Display name
	 */
	explicit ccCone(QString name = QString("Cone"));

	// ccHObject
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::CONE;
	}

	/**
	 * @brief Get the cone height
	 */
	inline PointCoordinateType getHeight() const
	{
		return m_height;
	}

	/**
	 * @brief Set the cone height
	 *
	 * Triggers mesh rebuild via updateRepresentation().
	 *
	 * @param[in] height New height
	 */
	void setHeight(PointCoordinateType height);

	/**
	 * @brief Get the bottom radius
	 */
	inline PointCoordinateType getBottomRadius() const
	{
		return m_bottomRadius;
	}

	/**
	 * @brief Set the bottom radius
	 *
	 * Triggers mesh rebuild.
	 *
	 * @param[in] radius New bottom radius
	 */
	virtual void setBottomRadius(PointCoordinateType radius);

	/**
	 * @brief Get the top radius
	 */
	inline PointCoordinateType getTopRadius() const
	{
		return m_topRadius;
	}

	/**
	 * @brief Set the top radius
	 *
	 * Triggers mesh rebuild.
	 *
	 * @param[in] radius New top radius (can be 0 for pointed)
	 */
	virtual void setTopRadius(PointCoordinateType radius);

	/**
	 * @brief Get the bottom center in world space
	 *
	 * Applies the transformation matrix to the local (0,0,0) point.
	 */
	virtual CCVector3 getBottomCenter() const;

	/**
	 * @brief Get the top center in world space
	 *
	 * Applies the transformation matrix to the local (0,0,height) point.
	 */
	virtual CCVector3 getTopCenter() const;

	/**
	 * @brief Get the center of the smaller-radius end cap
	 *
	 * @return Bottom center if bottomRadius <= topRadius, else top center
	 */
	virtual CCVector3 getSmallCenter() const;

	/**
	 * @brief Get the center of the larger-radius end cap
	 *
	 * @return Top center if topRadius >= bottomRadius, else bottom center
	 */
	virtual CCVector3 getLargeCenter() const;

	/**
	 * @brief Get the smaller of the two radii
	 */
	virtual PointCoordinateType getSmallRadius() const;

	/**
	 * @brief Get the larger of the two radii
	 */
	virtual PointCoordinateType getLargeRadius() const;

	/**
	 * @brief Check if the cone was created in snout mode
	 *
	 * Snout mode: m_xOff or m_yOff is non-zero, displacing
	 * the top cap center from the axis.
	 *
	 * @return true if snout mode
	 */
	virtual bool isSnoutMode() const
	{
		return (m_xOff != 0 || m_yOff != 0);
	}

	// ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Cone";
	}
	virtual bool hasDrawingPrecision() const override
	{
		return true;
	}
	virtual ccGenericPrimitive* clone() const override;

	/**
	 * @brief Compute the cone apex position (for pointed cones)
	 *
	 * @return Apex point in world space
	 */
	CCVector3 computeApex() const;

	/**
	 * @brief Compute the cone half angle
	 *
	 * @return Half angle in degrees
	 */
	double computeHalfAngle_deg() const;

  protected:
	// ccGenericPrimitive
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool buildUp() override;

	//! Radius at Z=0
	PointCoordinateType m_bottomRadius;
	//! Radius at Z=height
	PointCoordinateType m_topRadius;
	//! Snout X offset (displaces top center)
	PointCoordinateType m_xOff;
	//! Snout Y offset (displaces top center)
	PointCoordinateType m_yOff;
	//! Z extent
	PointCoordinateType m_height;
};
