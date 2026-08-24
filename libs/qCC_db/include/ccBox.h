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
 * @file ccBox.h
 *
 * @brief Axis-aligned 3D box primitive
 *
 * Represents a rectangular cuboid with 6 quad faces, 8 vertices,
 * and configurable width/height/depth dimensions.
 *
 * The box is axis-aligned in its local coordinate system (faces parallel
 * to XY/XZ/YZ planes). Use applyRigidTransformation() to rotate/translate
 * in world space.
 *
 * Mesh structure:
 * - 8 vertices at the 8 corners of the cuboid
 * - 12 triangles (2 per face) forming 6 quad faces
 * - Normals: one normal per face (flat shading)
 *
 * Uses:
 * - Primitive creation for geometric modeling
 * - Clipping operations (box clip tool)
 * - Bounding box visualization
 * - Reference geometry for measurements
 *
 * @extends ccGenericPrimitive
 */

#ifndef CC_BOX_PRIMITIVE_HEADER
#define CC_BOX_PRIMITIVE_HEADER

#include "ccGenericPrimitive.h"

/**
 * @class ccBox
 *
 * @brief Axis-aligned 3D box primitive
 *
 * A rectangular cuboid parameterized by width (X), height (Y), depth (Z).
 * The box is always axis-aligned in its local frame; rotation and translation
 * are applied via the inherited transformation from ccHObject.
 *
 * Mesh: flat-shaded 12-triangle mesh with per-face normals.
 *
 * @extends ccGenericPrimitive
 */
class QCC_DB_LIB_API ccBox : public ccGenericPrimitive
{
  public:
	/**
	 * @brief Create a box with specified dimensions
	 *
	 * @param[in] dims Box dimensions (width, height, depth) in local units
	 * @param[in] transMat Optional initial transformation matrix
	 * @param[in] name Display name in the DB tree
	 */
	ccBox(const CCVector3& dims,
	      const ccGLMatrix* transMat = nullptr,
	      QString name = QString("Box"));

	/**
	 * @brief Simplified constructor for the ccHObject factory
	 *
	 * @param[in] name Display name
	 */
	explicit ccBox(QString name = QString("Box"));

	// ccHObject

	/**
	 * @brief Returns CC_TYPES::BOX
	 */
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::BOX;
	}

	// ccGenericPrimitive

	/**
	 * @brief Returns \"Box\"
	 */
	virtual QString getTypeName() const override
	{
		return "Box";
	}

	/**
	 * @brief Clone this box
	 *
	 * @return New ccBox with the same dimensions and transformation
	 */
	virtual ccGenericPrimitive* clone() const override;

	/**
	 * @brief Set the box dimensions
	 *
	 * Rebuilds the mesh to match the new dimensions.
	 *
	 * @param[in] dims New dimensions (width, height, depth)
	 */
	inline void setDimensions(CCVector3& dims)
	{
		m_dims = dims;
		updateRepresentation();
	}

	/**
	 * @brief Get the current box dimensions
	 *
	 * @return Current width/height/depth
	 */
	const CCVector3& getDimensions() const
	{
		return m_dims;
	}

  protected:
	// Serialization
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	/**
	 * @brief Build the triangle mesh from current dimensions
	 *
	 * Creates 12 triangles (2 per face) at the 8 corners of the cuboid,
	 * with flat-shaded per-face normals.
	 */
	bool buildUp() override;

  private:
	//! Box dimensions (X=width, Y=height, Z=depth) in local coordinates
	CCVector3 m_dims;
};

#endif // CC_BOX_PRIMITIVE_HEADER
