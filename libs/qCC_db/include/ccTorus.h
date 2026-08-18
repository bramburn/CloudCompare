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

#ifndef CC_TORUS_PRIMITIVE_HEADER
#define CC_TORUS_PRIMITIVE_HEADER

/**
 * @file ccTorus.h
 *
 * @brief Torus primitive
 *
 * 3D torus (doughnut) primitive with circular or rectangular cross-section.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccGenericPrimitive.h"

/**
 * @brief Torus primitive
 *
 * 3D torus with configurable section type.
 */
class QCC_DB_LIB_API ccTorus : public ccGenericPrimitive
{
  public:
	//! Default drawing precision (angular steps)
	static const unsigned DEFAULT_DRAWING_PRECISION = 24;

	/**
	 * @brief Create a torus
	 * @param[in] insideRadius Inner radius
	 * @param[in] outsideRadius Outer radius
	 * @param[in] angle_rad Subtended angle (radians)
	 * @param[in] rectangularSection Rectangular cross-section
	 * @param[in] rectSectionHeight Section height (if rectangular)
	 * @param[in] transMat Optional transformation
	 * @param[in] name Torus name
	 * @param[in] precision Drawing precision
	 * @param[in] uniqueID Optional unique ID
	 */
	ccTorus(PointCoordinateType insideRadius,
	        PointCoordinateType outsideRadius,
	        double              angle_rad          = 2.0 * M_PI,
	        bool                rectangularSection = false,
	        PointCoordinateType rectSectionHeight  = 0,
	        const ccGLMatrix*   transMat           = nullptr,
	        QString             name               = QString("Torus"),
	        unsigned            precision          = DEFAULT_DRAWING_PRECISION,
	        unsigned            uniqueID           = ccUniqueIDGenerator::InvalidUniqueID);

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccTorus(QString name = QString("Torus"));

	//! Returns class ID
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::TORUS;
	}

	// inherited from ccGenericPrimitive
	QString getTypeName() const override
	{
		return "Torus";
	}
	bool hasDrawingPrecision() const override
	{
		return true;
	}
	ccGenericPrimitive* clone() const override;

  protected:
	// inherited from ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	//! Inside radius
	PointCoordinateType m_insideRadius;

	//! Outside radius
	PointCoordinateType m_outsideRadius;

	//! Whether torus has a rectangular (true) or circular (false) section
	bool m_rectSection;

	//! Rectangular section height (along Y-axis) if applicable
	PointCoordinateType m_rectSectionHeight;

	//! Subtended angle (in radians)
	double m_angle_rad;
};

#endif // CC_TORUS_PRIMITIVE_HEADER
