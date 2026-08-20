// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccBox.h
 *
 * @brief 3D box primitive for geometric modeling.
 *
 * @details Represents a 3D axis-aligned box primitive composed of
 * 6 rectangular plane faces.
 *
 * ## Overview
 *
 * A box is defined by:
 * - **Dimensions**: Width (X), Height (Y), Depth (Z)
 * - **Transformation**: Position and orientation
 *
 * Uses:
 * - Clipping operations
 * - Bounding box visualization
 * - Primitive creation for modeling
 * - Reference geometry
 *
 * ## Mesh Structure
 *
 * The box mesh consists of:
 * - 6 quad faces
 * - 8 vertices
 * - 24 vertex indices
 *
 * ## Usage
 *
 * @code
 * // Create a box
 * ccBox* box = new ccBox(CCVector3(1.0, 2.0, 3.0));
 *
 * // Apply transformation
 * ccGLMatrix mat;
 * mat.setTranslation(CCVector3(10, 0, 0));
 * box->applyRigidTransformation(mat);
 *
 * // Modify dimensions
 * CCVector3 dims = box->getDimensions();
 * dims.x *= 2; // Double width
 * box->setDimensions(dims);
 *
 * // Add to scene
 * dbRoot->addChild(box);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGenericPrimitive for base class
 */

#ifndef CC_BOX_PRIMITIVE_HEADER
#define CC_BOX_PRIMITIVE_HEADER

// Local
#include "ccGenericPrimitive.h"

/**
 * @brief 3D box primitive.
 *
 * @details A box is an axis-aligned rectangular cuboid
 * composed of 6 plane faces.
 *
 * Features:
 * - Configurable dimensions
 * - Rigid transformation support
 * - Mesh generation
 * - Serialization
 *
 * @extends ccGenericPrimitive
 */
class QCC_DB_LIB_API ccBox : public ccGenericPrimitive
{
  public:
	/**
	 * @brief Create a box with dimensions.
	 *
	 * @param[in] dims Box dimensions (width, height, depth).
	 * @param[in] transMat Optional transformation matrix.
	 * @param[in] name Box name.
	 */
	ccBox(const CCVector3&  dims,
	      const ccGLMatrix* transMat = nullptr,
	      QString           name     = QString("Box"));

	/**
	 * @brief Simplified constructor.
	 *
	 * @param[in] name Box name.
	 *
	 * @note For ccHObject factory only.
	 */
	ccBox(QString name = QString("Box"));

	/**
	 * @brief Get class type.
	 */
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::BOX;
	}

	// from ccGenericPrimitive
	/**
	 * @brief Get type name.
	 */
	virtual QString getTypeName() const override
	{
		return "Box";
	}

	/**
	 * @brief Create a clone of this box.
	 */
	virtual ccGenericPrimitive* clone() const override;

	/**
	 * @brief Set box dimensions.
	 *
	 * @param[in] dims New dimensions (width, height, depth).
	 *
	 * @note Updates the mesh representation.
	 */
	inline void setDimensions(CCVector3& dims)
	{
		m_dims = dims;
		updateRepresentation();
	}

	/**
	 * @brief Get box dimensions.
	 *
	 * @return Current dimensions.
	 */
	const CCVector3& getDimensions() const
	{
		return m_dims;
	}

  protected:
	// Serialization
	/**
	 * @brief Serialize to file.
	 */
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;

	/**
	 * @brief Deserialize from file.
	 */
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;

	/**
	 * @brief Minimum file version.
	 */
	short minimumFileVersion_MeOnly() const override;

	/**
	 * @brief Build the box mesh.
	 */
	bool buildUp() override;

  private:
	//! Box dimensions (width, height, depth).
	CCVector3 m_dims;
};

#endif // CC_BOX_PRIMITIVE_HEADER
