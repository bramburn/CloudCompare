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

#ifndef CC_EXTRU_PRIMITIVE_HEADER
#define CC_EXTRU_PRIMITIVE_HEADER

/**
 * @file ccExtru.h
 *
 * @brief Extrusion primitive
 *
 * 3D extrusion from a 2D profile (polyline) along the Z axis.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccGenericPrimitive.h"

/**
 * @brief Extrusion primitive
 *
 * 3D mesh created by extruding a 2D profile along Z.
 */
class QCC_DB_LIB_API ccExtru : public ccGenericPrimitive
{
  public:
	/**
	 * @brief Create an extrusion
	 * @param[in] profile 2D profile in XY plane
	 * @param[in] height Extrusion height along Z
	 * @param[in] transMat Transformation matrix
	 * @param[in] name Extrusion name
	 */
	ccExtru(const std::vector<CCVector2>& profile,
	        PointCoordinateType height,
	        const ccGLMatrix* transMat = nullptr,
	        QString name = QString("Extrusion"));

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccExtru(QString name = QString("Extrusion"));

	//! Returns class ID
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::EXTRU;
	}

	// inherited from ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Extrusion";
	}
	virtual ccGenericPrimitive* clone() const override;

	//! Returns extrusion thickness
	const PointCoordinateType getThickness() const
	{
		return m_height;
	}

	//! Returns profile
	const std::vector<CCVector2>& getProfile() const
	{
		return m_profile;
	}

  protected:
	// inherited from ccGenericPrimitive
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool buildUp() override;

	//! Extrusion thickness
	PointCoordinateType m_height;

	//! Profile
	std::vector<CCVector2> m_profile;
};

#endif // CC_EXTRU_PRIMITIVE_HEADER
