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
// #          COPYRIGHT: CNRS / OSERen / University of Rennes, France       #
// #                                                                        #
// ##########################################################################

/**
 * @file ccDisc.h
 *
 * @brief Disc primitive class
 *
 * Represents a 3D disc (filled circle) primitive.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include <ccGenericPrimitive.h>

/**
 * @brief Disc primitive
 *
 * A 3D filled disc (disk) primitive.
 */
class QCC_DB_LIB_API ccDisc : public ccGenericPrimitive
{
  public:
	//! Default drawing precision (angular steps)
	static const unsigned DEFAULT_DRAWING_PRECISION = 72;

	/**
	 * @brief Create a disc
	 * @param[in] radius Disc radius
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Disc name
	 * @param[in] precision Drawing precision
	 * @param[in] uniqueID Optional unique ID
	 */
	ccDisc(PointCoordinateType radius,
	       const ccGLMatrix* transMat = nullptr,
	       QString name = QString("Disc"),
	       unsigned precision = DEFAULT_DRAWING_PRECISION,
	       unsigned uniqueID = ccUniqueIDGenerator::InvalidUniqueID);

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccDisc(QString name = QString("Disc"));

	//! Returns radius
	inline PointCoordinateType getRadius() const
	{
		return m_radius;
	}
	//! Sets radius
	/** \warning changes primitive content (calls ccGenericPrimitive::updateRepresentation)
	 **/
	void setRadius(PointCoordinateType radius);

	//! Returns class ID
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::DISC;
	}

	// inherited from ccGenericPrimitive
	QString getTypeName() const override
	{
		return "Disc";
	}
	bool hasDrawingPrecision() const override
	{
		return true;
	}
	ccGenericPrimitive* clone() const override;

  protected:
	// inherited from ccGenericPrimitive
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool buildUp() override;

	//! Radius
	PointCoordinateType m_radius;
};
