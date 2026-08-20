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
// #                                                                        //
// ##########################################################################

/**
 * @file ccAdvancedTypes.h
 *
 * @brief Advanced container types for per-point/per-triangle data.
 *
 * @details Defines specialized array types for storing normals, colors,
 * texture coordinates, and other per-element data.
 *
 * ## Container Types
 *
 * ### Normals
 * - **NormsIndexesTableType**: Compressed normal indices
 * - **NormsTableType**: Uncompressed 3D normals
 *
 * ### Colors
 * - **ColorsTableType**: RGB colors
 * - **RGBAColorsTableType**: RGBA colors with alpha
 *
 * ### Texture Coordinates
 * - **TextureCoordsContainer**: 2D UV coordinates
 *
 * ## Storage Format
 *
 * | Type | Components | Storage |
 * |------|-----------|--------|
 * | Normals (compressed) | 1 | Index to normal table |
 * | Normals (float) | 3 (Nx,Ny,Nz) | 12 bytes |
 * | RGB | 3 (R,G,B) | 3 bytes |
 * | RGBA | 4 (R,G,B,A) | 4 bytes |
 * | TexCoords | 2 (U,V) | 8 bytes |
 *
 * ## Usage
 *
 * @code
 * // Access colors on a point cloud
 * ColorsTableType* colors = cloud->getColors();
 * if (colors) {
 *     ccColor::Rgb color = colors->getValue(pointIndex);
 * }
 *
 * // Access normals
 * NormsTableType* normals = cloud->getNorms();
 * if (normals) {
 *     CCVector3 normal = normals->getValue(pointIndex);
 * }
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccArray for base template
 * @see ccColorTypes for color definitions
 */

#ifndef CC_ADVANCED_TYPES_HEADER
#define CC_ADVANCED_TYPES_HEADER

// Local
#include "ccArray.h"
#include "ccColorTypes.h"
#include "ccNormalCompressor.h"

/**
 * @brief Compressed normal indices array.
 *
 * @details Stores indices into the compressed normal table
 * for efficient normal storage.
 *
 * @extends ccArray<CompressedNormType, 1, CompressedNormType>
 */
class NormsIndexesTableType : public ccArray<CompressedNormType, 1, CompressedNormType>
{
  public:
	/**
	 * @brief Default constructor.
	 */
	QCC_DB_LIB_API NormsIndexesTableType();

	~NormsIndexesTableType() override = default;

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::NORMAL_INDEXES_ARRAY;
	}

	/**
	 * @brief Minimum file version.
	 */
	short minimumFileVersion() const override
	{
		return 41;
	}

	/**
	 * @brief Clone the array.
	 */
	NormsIndexesTableType* clone() override
	{
		NormsIndexesTableType* cloneArray = new NormsIndexesTableType();
		if (!copy(*cloneArray))
		{
			ccLog::Warning("[NormsIndexesTableType::clone] Failed to clone array");
			cloneArray->release();
			return nullptr;
		}
		cloneArray->setName(getName());
		return cloneArray;
	}

	/**
	 * @brief Deserialize from file.
	 */
	QCC_DB_LIB_API bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
};

/**
 * @brief Uncompressed normals array.
 *
 * @details Stores 3D normals as float vectors.
 *
 * @extends ccArray<CCVector3, 3, PointCoordinateType>
 */
class NormsTableType : public ccArray<CCVector3, 3, PointCoordinateType>
{
  public:
	/**
	 * @brief Default constructor.
	 */
	NormsTableType()
	    : ccArray<CCVector3, 3, PointCoordinateType>("Normals")
	{
	}

	~NormsTableType() = default;

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::NORMALS_ARRAY;
	}

	/**
	 * @brief Clone the array.
	 */
	NormsTableType* clone() override
	{
		NormsTableType* cloneArray = new NormsTableType();
		if (!copy(*cloneArray))
		{
			ccLog::Warning("[NormsTableType::clone] Failed to clone array");
			cloneArray->release();
			return nullptr;
		}
		cloneArray->setName(getName());
		return cloneArray;
	}
};

/**
 * @brief RGB colors array.
 *
 * @details Stores RGB colors per element.
 *
 * @extends ccArray<ccColor::Rgb, 3, ColorCompType>
 */
class ColorsTableType : public ccArray<ccColor::Rgb, 3, ColorCompType>
{
  public:
	/**
	 * @brief Default constructor.
	 */
	ColorsTableType()
	    : ccArray<ccColor::Rgb, 3, ColorCompType>("RGB colors")
	{
	}

	~ColorsTableType() = default;

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::RGB_COLOR_ARRAY;
	}

	/**
	 * @brief Clone the array.
	 */
	ColorsTableType* clone() override
	{
		ColorsTableType* cloneArray = new ColorsTableType();
		if (!copy(*cloneArray))
		{
			ccLog::Warning("[ColorsTableType::clone] Failed to clone array");
			cloneArray->release();
			return nullptr;
		}
		cloneArray->setName(getName());
		return cloneArray;
	}
};

/**
 * @brief RGBA colors array.
 *
 * @details Stores RGBA colors with alpha channel.
 *
 * @extends ccArray<ccColor::Rgba, 4, ColorCompType>
 */
class RGBAColorsTableType : public ccArray<ccColor::Rgba, 4, ColorCompType>
{
  public:
	/**
	 * @brief Default constructor.
	 */
	RGBAColorsTableType()
	    : ccArray<ccColor::Rgba, 4, ColorCompType>("RGBA colors")
	{
	}

	~RGBAColorsTableType() = default;

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::RGBA_COLOR_ARRAY;
	}

	/**
	 * @brief Clone the array.
	 */
	RGBAColorsTableType* clone() override
	{
		RGBAColorsTableType* cloneArray = new RGBAColorsTableType();
		if (!copy(*cloneArray))
		{
			ccLog::Warning("[RGBAColorsTableType::clone] Failed to clone array");
			cloneArray->release();
			return nullptr;
		}
		cloneArray->setName(getName());
		return cloneArray;
	}
};

/**
 * @brief 2D texture coordinates.
 *
 * @details Stores (U, V) texture coordinates.
 */
struct TexCoords2D
{
	/**
	 * @brief Default constructor.
	 */
	TexCoords2D()
	    : tx(-1.0f)
	    , ty(-1.0f)
	{
	}

	/**
	 * @brief Construct from values.
	 */
	TexCoords2D(float x, float y)
	    : tx(x)
	    , ty(y)
	{
	}

	union
	{
		//! UV components.
		struct
		{
			float tx, ty;
		};

		//! Array access.
		float t[2];
	};
};

/**
 * @brief Texture coordinates array.
 *
 * @details Stores 2D UV texture coordinates.
 *
 * @extends ccArray<TexCoords2D, 2, float>
 */
class TextureCoordsContainer : public ccArray<TexCoords2D, 2, float>
{
  public:
	/**
	 * @brief Default constructor.
	 */
	TextureCoordsContainer()
	    : ccArray<TexCoords2D, 2, float>("Texture coordinates")
	{
	}

	~TextureCoordsContainer() = default;

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::TEX_COORDS_ARRAY;
	}

	/**
	 * @brief Clone the array.
	 */
	TextureCoordsContainer* clone() override
	{
		TextureCoordsContainer* cloneArray = new TextureCoordsContainer();
		if (!copy(*cloneArray))
		{
			ccLog::Warning("[TextureCoordsContainer::clone] Failed to clone array");
			cloneArray->release();
			return nullptr;
		}
		cloneArray->setName(getName());
		return cloneArray;
	}
};

#endif // CC_ADVANCED_TYPES_HEADER
