/**
 * @file LasMetadata.h
 *
 * @brief LAS metadata handling utilities.
 *
 * @details Functions for saving and loading LAS metadata
 * to/from CloudCompare point clouds.
 *
 * ## Metadata Keys
 *
 * - Scale factors (X, Y, Z)
 * - Offset values (X, Y, Z)
 * - Version (major, minor)
 * - Point format
 * - Global encoding
 * - VLRs (Variable Length Records)
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
	constexpr const char X_SCALE[]           = "LAS.scale.x";
	//! Y scale metadata key.
	constexpr const char Y_SCALE[]           = "LAS.scale.y";
	//! Z scale metadata key.
	constexpr const char Z_SCALE[]           = "LAS.scale.z";
	//! X offset metadata key.
	constexpr const char X_OFFSET[]          = "LAS.offset.x";
	//! Y offset metadata key.
	constexpr const char Y_OFFSET[]          = "LAS.offset.y";
	//! Z offset metadata key.
	constexpr const char Z_OFFSET[]          = "LAS.offset.z";
	//! Version major metadata key.
	constexpr const char VERSION_MAJOR[]     = "LAS.version.major";
	//! Version minor metadata key.
	constexpr const char VERSION_MINOR[]     = "LAS.version.minor";
	//! Point format metadata key.
	constexpr const char POINT_FORMAT[]      = "LAS.point_format";
	//! Global encoding metadata key.
	constexpr const char GLOBAL_ENCODING[]   = "LAS.global_encoding";
	//! Project UUID metadata key.
	constexpr const char PROJECT_UUID[]      = "LAS.project_uuid";
	//! System identifier metadata key.
	constexpr const char SYSTEM_IDENTIFIER[] = "LAS.system_identifier";
	//! VLRs metadata key.
	constexpr const char VLRS[]              = "LAS.variableLengthRecords";
	//! Extra fields metadata key.
	constexpr const char EXTRA_FIELDS[]      = "LAS.extra_fields";

	/**
	 * @brief Save metadata into point cloud.
	 *
	 * @param[in] header LAS header.
	 * @param[out] pointCloud Point cloud.
	 * @param[in] extraScalarFields Extra scalar fields.
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
