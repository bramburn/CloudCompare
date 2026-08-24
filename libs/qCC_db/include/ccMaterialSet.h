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
 * @file ccMaterialSet.h
 *
 * @brief Material set (library) for mesh textures
 *
 * Manages a collection of materials for mesh rendering.
 * Supports MTL file format (Wavefront .mtl).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "CCShareable.h"
#include "ccHObject.h"

class ccGenericGLDisplay;

/**
 * @brief Material set/library
 *
 * Collection of materials with MTL file support.
 */
class QCC_DB_LIB_API ccMaterialSet : public std::vector<ccMaterial::CShared>
    , public CCShareable
    , public ccHObject
{
  public:
	/**
	 * @brief Create a material set
	 * @param[in] name Set name
	 */
	ccMaterialSet(const QString& name = QString());

	// inherited from ccHObject
	/**
	 * @brief Get class type
	 * @return CC_TYPES::MATERIAL_SET
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::MATERIAL_SET;
	}
	/**
	 * @brief Check if shareable
	 * @return true
	 */
	bool isShareable() const override
	{
		return true;
	}

	/**
	 * @brief Find material by name
	 * @param[in] mtlName Material name
	 * @return Material index, or -1 if not found
	 */
	int findMaterialByName(const QString& mtlName);

	/**
	 * @brief Find material by unique ID
	 * @param[in] uniqueID Unique identifier
	 * @return Material index, or -1 if not found
	 */
	int findMaterialByUniqueID(const QString& uniqueID);

	/**
	 * @brief Add a material
	 * @param[in] mat Material to add
	 * @param[in] allowDuplicateNames Allow duplicate names
	 * @return Material index
	 */
	int addMaterial(ccMaterial::CShared mat, bool allowDuplicateNames = false);

	/**
	 * @brief Parse MTL file
	 * @param[in] path Base path
	 * @param[in] filename MTL filename
	 * @param[out] materials Parsed materials
	 * @param[out] errors Error messages
	 * @return true on success
	 */
	static bool ParseMTL(const QString& path, const QString& filename, ccMaterialSet& materials, QStringList& errors);

	/**
	 * @brief Save to MTL file
	 * @param[in] path Output path
	 * @param[in] baseFilename Base filename
	 * @param[out] errors Error messages
	 * @return true on success
	 */
	bool saveAsMTL(const QString& path, const QString& baseFilename, QStringList& errors) const;

	/**
	 * @brief Clone the material set
	 * @return New material set
	 */
	ccMaterialSet* clone() const;

	/**
	 * @brief Append materials from another set
	 * @param[in] source Source set
	 * @return true on success
	 */
	bool append(const ccMaterialSet& source);

	// inherited from ccSerializableObject
	bool isSerializable() const override
	{
		return true;
	}

  protected:
	// inherited from ccHObject
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	//! Default destructor (protected: use 'release' instead)
	~ccMaterialSet() override = default;
};
