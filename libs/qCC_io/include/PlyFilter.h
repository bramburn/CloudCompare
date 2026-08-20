// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
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
 * @file PlyFilter.h
 *
 * @brief PLY (Polygon Library) file filter.
 *
 * @details Stanford PLY file format filter for loading and saving
 * point clouds and meshes.
 *
 * ## Overview
 *
 * PlyFilter supports:
 * - ASCII and binary PLY formats
 * - Point clouds and meshes
 * - Custom properties and colors
 * - Texture coordinates
 *
 * ## Usage
 *
 * @code
 * PlyFilter filter;
 * CC_FILE_ERROR err = filter.loadFile("cloud.ply", container, params);
 *
 * // Set output format
 * PlyFilter::SetDefaultOutputFormat(PLY_ASCII);
 * PlyFilter::SetAddSFPrefix(true);
 * filter.saveToFile(entity, "output.ply", saveParams);
 * @endcode
 *
 * @extends FileIOFilter
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_PLY_FILTER_HEADER
#define CC_PLY_FILTER_HEADER

#include "FileIOFilter.h"
#include "rply.h"

//! PLY format type names.
static const char e_ply_type_names[][12] = {
    "PLY_INT8",
    "PLY_UINT8",
    "PLY_INT16",
    "PLY_UINT16",
    "PLY_INT32",
    "PLY_UIN32",
    "PLY_FLOAT32",
    "PLY_FLOAT64",
    "PLY_CHAR",
    "PLY_UCHAR",
    "PLY_SHORT",
    "PLY_USHORT",
    "PLY_INT",
    "PLY_UINT",
    "PLY_FLOAT",
    "PLY_DOUBLE",
    "PLY_LIST"};

//! PLY storage mode names.
static const char e_ply_storage_mode_names[][24] =
    {"PLY_BIG_ENDIAN", "PLY_LITTLE_ENDIAN", "PLY_ASCII", "PLY_DEFAULT"};

/**
 * @brief PLY property structure.
 */
struct plyProperty
{
	//! PLY property handle.
	p_ply_property prop;

	//! Property name.
	const char* propName;

	//! Property type.
	e_ply_type type;

	//! List length type.
	e_ply_type length_type;

	//! List value type.
	e_ply_type value_type;

	//! Element index.
	int elemIndex;
};

/**
 * @brief PLY element structure.
 */
struct plyElement
{
	//! PLY element handle.
	p_ply_element elem;

	//! Element name.
	const char* elementName;

	//! Number of instances.
	long elementInstances;

	//! Properties.
	std::vector<plyProperty> properties;

	//! Property count.
	int propertiesCount;

	//! Is face element.
	bool isFace;
};

/**
 * @brief PLY file filter.
 *
 * @details Stanford PLY file format filter.
 *
 * @extends FileIOFilter
 */
class QCC_IO_LIB_API PlyFilter : public FileIOFilter
{
  public:
	/**
	 * @brief Create PLY filter.
	 */
	PlyFilter();

	/**
	 * @brief Set default output format.
	 *
	 * @param[in] format Storage format.
	 */
	static void SetDefaultOutputFormat(e_ply_storage_mode format);

	/**
	 * @brief Set scalar field prefix.
	 *
	 * @param[in] state Add 'scalar_' prefix to SF names.
	 */
	static void SetAddSFPrefix(bool state);

	// inherited from FileIOFilter
	/**
	 * @brief Load PLY file.
	 *
	 * @param[in] filename File path.
	 * @param[out] container Entity container.
	 * @param[in] parameters Load parameters.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/**
	 * @brief Check if can save.
	 *
	 * @param[in] type Entity type.
	 * @param[out] multiple Multiple objects.
	 * @param[out] exclusive Exclusive save.
	 *
	 * @return true if can save.
	 */
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;

	/**
	 * @brief Save to PLY file.
	 *
	 * @param[in] entity Entity to save.
	 * @param[in] filename File path.
	 * @param[in] parameters Save parameters.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;

	/**
	 * @brief Load with texture.
	 *
	 * @param[in] filename PLY file path.
	 * @param[in] textureFilename Texture file path.
	 * @param[out] container Entity container.
	 * @param[in] parameters Load parameters.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR loadFile(const QString& filename, const QString& textureFilename, ccHObject& container, LoadParameters& parameters);

  private:
	/**
	 * @brief Internal save method.
	 *
	 * @param[in] entity Entity to save.
	 * @param[in] filename File path.
	 * @param[in] storageType Storage format.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR saveToFile(ccHObject* entity, QString filename, e_ply_storage_mode storageType);
};

#endif // CC_PLY_FILTER_HEADER
