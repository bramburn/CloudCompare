#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        #
// ##########################################################################

/**
 * @file LasIOFilter.h
 *
 * @brief LAS/LAZ LiDAR file I/O filter
 *
 * FileIOFilter subclass for reading and writing ASPRS LAS/LAZ files
 * (the standard interchange format for LiDAR point cloud data).
 *
 * ## Supported Formats
 *
 * - **LAS**: ASPRS LAS format (binary, uncompressed). Read/write.
 * - **LAZ**: LASzip-compressed LAS (lossless). Read/write.
 *
 * ## Data Mapping
 *
 * Standard LAS point fields:
 * - X, Y, Z → ccPointCloud coordinates
 * - Intensity → Scalar field (displayed as grey intensity by default)
 * - Return Number / Number of Returns → Scalar fields
 * - Scan Angle / Scan Direction / Flight Edge → Scalar fields
 * - Classification → Scalar field (and CC class labels via LasDetails)
 * - Color (RGB/NIR) → ccPointCloud per-point colors
 * - GPS Time → Scalar field
 *
 * Extended (extra) attributes: stored as extra scalar fields via
 * LasExtraScalarField, preserving EPT-style extended attributes.
 *
 * ## File Versions
 *
 * Supports all LAS versions (1.0–1.4) and all point formats
 * (0–10). Version and format are tracked per-file via LasDetails.
 *
 * ## Dialog
 *
 * On load: shows LasOpenDialog for per-file options (coordinate
 * shift, scale, classification set, extra fields). Remembers last
 * settings via persistent settings.
 *
 * On save: exports the current cloud to LAS 1.4 format with
 * configurable precision (scale factors) and compression.
 *
 * @extends FileIOFilter
 * @see https://www.asprs.org/laser-laser-file-format-exchange-activities
 */

#include "FileIOFilter.h"
#include "LasDetails.h"
#include "LasExtraScalarField.h"
#include "LasOpenDialog.h"

// System
#include <memory>

/**
 * @class LasIOFilter
 *
 * @brief LAS/LAZ LiDAR file I/O filter
 *
 * Provides read/write for ASPRS LAS format and LASzip-compressed
 * LAZ files. Registered with FileIOFilter automatically via
 * qLASIO's registerFileIOFilter().
 *
 * @extends FileIOFilter
 */
class LasIOFilter : public FileIOFilter
{
  public:
	//! Construct the filter
	LasIOFilter();

	// FileIOFilter

	/**
	 * @brief Load a LAS/LAZ file into a ccHObject hierarchy
	 *
	 * Shows LasOpenDialog (unless SILENT mode) for per-file load options.
	 * Parses the LAS public header and point records. Maps standard
	 * LAS fields to ccPointCloud entities. Stores extra attribute info
	 * in m_infoOfLastOpened for round-trip preservation.
	 *
	 * @param[in] fileName Path to the .las or .laz file
	 * @param[out] container Root of the loaded hierarchy (contains the point cloud)
	 * @param[in] parameters Load parameters (progress callback, shift mode, etc.)
	 * @return CC_FILE_ERROR status
	 */
	CC_FILE_ERROR loadFile(const QString& fileName, ccHObject& container, LoadParameters& parameters) override;

	/**
	 * @brief Check if this filter can save a given entity type
	 *
	 * Can save: POINT_CLOUD, MESH group (with mesh colors as intensity)
	 *
	 * @param[in] type Entity type
	 * @param[out] multiple Whether multiple entities are supported
	 * @param[out] exclusive Whether this is the exclusive save format
	 * @return true if the filter can save this type
	 */
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;

	/**
	 * @brief Save an entity to a LAS/LAZ file
	 *
	 * Exports to LAS 1.4 format. Supports:
	 * - Coordinate scaling (precision control)
	 * - Coordinate shifting (for large files)
	 * - Intensity export from scalar field
	 * - RGB color export
	 * - Classification export
	 *
	 * @param[in] entity Entity to save (typically a point cloud)
	 * @param[in] filename Output file path
	 * @param[in] parameters Save parameters
	 * @return CC_FILE_ERROR status
	 */
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;

  private:
	/**
	 * @struct FileInfo
	 *
	 * @brief Metadata for a loaded LAS file
	 *
	 * Stores version info and extra attribute metadata to enable
	 * round-trip read/write without loss of extended attributes.
	 */
	struct FileInfo
	{
		//! LAS format version (major.minor)
		LasDetails::LasVersion version;
		//! Extra scalar fields (extended attributes)
		std::vector<LasExtraScalarField> extraScalarFields;

		/**
		 * @brief Check equality (for round-trip validation)
		 *
		 * Compares version, field count, and all field metadata
		 * (name, type, offset, size, options).
		 */
		bool operator==(const FileInfo& other)
		{
			bool versionIsSame = version.minorVersion == other.version.minorVersion
			                     && version.pointFormat == other.version.pointFormat;
			if (!versionIsSame)
			{
				return false;
			}

			if (extraScalarFields.size() != other.extraScalarFields.size())
			{
				return false;
			}

			for (size_t i = 0; i < extraScalarFields.size(); ++i)
			{
				const auto& lhs = extraScalarFields[i];
				const auto& rhs = other.extraScalarFields[i];

				if (strncmp(lhs.name, rhs.name, LasExtraScalarField::MAX_NAME_SIZE) != 0
				    || lhs.type != rhs.type
				    || lhs.byteOffset != rhs.byteOffset
				    || lhs.numElements() != rhs.numElements()
				    || lhs.options != rhs.options)
				{
					return false;
				}
			}
			return true;
		}

		//! Inequality
		bool operator!=(const FileInfo& other)
		{
			return !(*this == other);
		}
	};

	//! Metadata for the last opened file (for round-trip)
	std::unique_ptr<FileInfo> m_infoOfLastOpened;
	//! Per-file open dialog
	LasOpenDialog m_openDialog{};
};
