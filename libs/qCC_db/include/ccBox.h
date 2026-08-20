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

#ifndef CC_BOX_PRIMITIVE_HEADER
#define CC_BOX_PRIMITIVE_HEADER

/**
 * @file ccBox.h
 *
 * @brief Box primitive class
 *
 * Represents a 3D box primitive composed of 6 plane faces.
 * Can be used for clipping, visualization bounding boxes, etc.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccGenericPrimitive.h"

/**
 * @brief Box primitive
 *
 * A 3D box composed of 6 plane faces.
 */
class QCC_DB_LIB_API ccBox : public ccGenericPrimitive
{
  public:
	/**
	 * @brief Create a box
	 * @param[in] dims Box dimensions (width, height, depth)
	 * @param[in] transMat Optional transformation matrix
	 * @param[in] name Box name
	 */
	ccBox(const CCVector3&  dims,
	      const ccGLMatrix* transMat = nullptr,
	      QString           name     = QString("Box"));

	//! Simplified constructor
	/** For ccHObject factory only!
	 **/
	ccBox(QString name = QString("Box"));

	//! Returns class ID
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::BOX;
	}

	// inherited from ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Box";
	}
	virtual ccGenericPrimitive* clone() const override;

	//! Sets box dimensions
	inline void setDimensions(CCVector3& dims)
	{
		m_dims = dims;
		updateRepresentation();
	}

	//! Returns box dimensions
	const CCVector3& getDimensions() const
	{
		return m_dims;
	}

  protected:
	// inherited from ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	//! Box dimensions
	CCVector3 m_dims;
};

#endif // CC_BOX_PRIMITIVE_HEADER
