/**
 * @file LasMetadata.h
 *
 * @brief LAS metadata persistence — round-trip between LAS header and ccPointCloud metadata
 *
 * Stores and retrieves LAS file metadata to/from ccPointCloud::metaData().
 * This enables round-trip LAS read/write without losing header information.
 *
 * ## Metadata Schema
 *
 * | Key                    | Type     | Description                        |
 * |------------------------|----------|------------------------------------|
 * | LAS.scale.x/y/z        | double   | Coordinate scale factors           |
 * | LAS.offset.x/y/z        | double   | Coordinate offsets                 |
 * | LAS.version.major      | int      | LAS major version (always 1)        |
 * | LAS.version.minor      | int      | LAS minor version (2, 3, or 4)     |
 * | LAS.point_format       | int      | Point format ID (0-10)             |
 * | LAS.global_encoding    | uint16_t | GPS time type, WKT, etc.           |
 * | LAS.project_uuid       | QString  | Unique file identifier             |
 * | LAS.system_identifier  | QString  | Software that created the file      |
 * | LAS.variableLengthRecords | QByteArray | Serialized VLR data           |
 * | LAS.extra_fields       | QString  | Extra attribute definitions       |
 *
 * ## Coordinate Conversion
 *
 * LAS stores coordinates as 32-bit integers. Real-world coordinates are:
 * `real = stored * scale + offset`
 *
 * When loading: `stored = (real - offset) / scale`
 *
 * @author Thomas Montaigu
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        //
// ##########################################################################

struct laszip_header;
class ccPointCloud;

#include "LasDetails.h"
#include "LasExtraScalarField.h"
#include "LasVlr.h"

// CCCoreLib
#include <CCGeom.h>
// LASzip
#include <laszip/laszip_api.h>

/**
 * @brief LAS metadata handling namespace.
 */
namespace LasMetadata
{
	//! X scale metadata key.
	constexpr const char X_SCALE[] = "LAS.scale.x";
	//! Y scale metadata key.
	constexpr const char Y_SCALE[] = "LAS.scale.y";
	//! Z scale metadata key.
	constexpr const char Z_SCALE[] = "LAS.scale.z";
	//! X offset metadata key.
	constexpr const char X_OFFSET[] = "LAS.offset.x";
	//! Y offset metadata key.
	constexpr const char Y_OFFSET[] = "LAS.offset.y";
	//! Z offset metadata key.
	constexpr const char Z_OFFSET[] = "LAS.offset.z";
	//! Version major metadata key.
	constexpr const char VERSION_MAJOR[] = "LAS.version.major";
	//! Version minor metadata key.
	constexpr const char VERSION_MINOR[] = "LAS.version.minor";
	//! Point format metadata key.
	constexpr const char POINT_FORMAT[] = "LAS.point_format";
	//! Global encoding metadata key.
	constexpr const char GLOBAL_ENCODING[] = "LAS.global_encoding";
	//! Project UUID metadata key.
	constexpr const char PROJECT_UUID[] = "LAS.project_uuid";
	//! System identifier metadata key.
	constexpr const char SYSTEM_IDENTIFIER[] = "LAS.system_identifier";
	//! VLRs metadata key.
	constexpr const char VLRS[] = "LAS.variableLengthRecords";
	//! Extra fields metadata key.
	constexpr const char EXTRA_FIELDS[] = "LAS.extra_fields";

	/**
	 * @brief Save LAS header metadata into a point cloud
	 *
	 * Copies scale, offset, version, point format, global encoding,
	 * VLRs, and extra field definitions into the cloud's metadata.
	 *
	 * @param[in] header Source LAS header
	 * @param[out] pointCloud Target CloudCompare point cloud
	 * @param[in] extraScalarFields Extra attribute definitions to store
	 */
	void SaveMetadataInto(const laszip_header& header, ccPointCloud& pointCloud, const std::vector<LasExtraScalarField>& extraScalarFields);

	/**
	 * @brief Load VLRs from point cloud.
	 *
	 * @param[in] pointCloud Point cloud.
	 * @param[out] vlr VLR container.
	 *
	 * @return true if successful.
	 */
	bool LoadVlrs(const ccPointCloud& pointCloud, LasVlr& vlr);

	/**
	 * @brief Load scale from point cloud.
	 *
	 * @param[in] pointCloud Point cloud.
	 * @param[out] scale Scale vector.
	 *
	 * @return true if successful.
	 */
	bool LoadScaleFrom(const ccPointCloud& pointCloud, CCVector3d& scale);

	/**
	 * @brief Load offset from point cloud.
	 *
	 * @param[in] pointCloud Point cloud.
	 * @param[out] offset Offset vector.
	 *
	 * @return true if successful.
	 */
	bool LoadOffsetFrom(const ccPointCloud& pointCloud, CCVector3d& offset);

	/**
	 * @brief Load LAS version from point cloud.
	 *
	 * @param[in] pointCloud Point cloud.
	 * @param[out] version LAS version.
	 *
	 * @return true if successful.
	 */
	bool LoadLasVersionFrom(const ccPointCloud& pointCloud, LasDetails::LasVersion& version);

	/**
	 * @brief Load global encoding from point cloud.
	 *
	 * @param[in] pointCloud Point cloud.
	 * @param[out] outGlobalEncoding Global encoding.
	 *
	 * @return true if successful.
	 */
	bool LoadGlobalEncoding(const ccPointCloud& pointCloud, uint16_t& outGlobalEncoding);

	/**
	 * @brief Load project UUID into header.
	 *
	 * @param[in] pointCloud Point cloud.
	 * @param[out] header LAS header.
	 *
	 * @return true if successful.
	 */
	bool LoadProjectUUID(const ccPointCloud& pointCloud, laszip_header& header);
} // namespace LasMetadata
