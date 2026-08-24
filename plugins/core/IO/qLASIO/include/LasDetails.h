/**
 * @file LasDetails.h
 *
 * @brief LAS format details and utilities.
 *
 * @details Core definitions for LAS/LAZ file handling.
 *
 * ## Features
 *
 * - LAS version and point format constants
 * - Point format size calculations
 * - Extended VLR header structures
 * - Chunk interval management (COPC)
 * - Version selection helpers
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

// CCCorelib
#include <BoundingBox.h>
#include <CCTypes.h>

// Qt
#include <QtGlobal>

// System
#include <array>
#include <cstdint>
#include <limits>
#include <vector>

class ccPointCloud;
class ccScalarField;

class QDataStream;

struct laszip_header;
struct laszip_vlr;
typedef laszip_vlr laszip_vlr_struct;

constexpr size_t LAS_VLR_HEADER_SIZE = 54;    //!< VLR header size.
constexpr double SCAN_ANGLE_SCALE    = 0.006; //!< Scan angle scale factor.

/**
 * @brief LAS standard dimension names.
 *
 * Standard dimension names as defined by the LAS specification.
 */
namespace LasNames
{
	//! Intensity field name.
	constexpr const char* Intensity = "Intensity";
	//! Return number field name.
	constexpr const char* ReturnNumber = "Return Number";
	//! Number of returns field name.
	constexpr const char* NumberOfReturns = "Number Of Returns";
	//! Scan direction flag field name.
	constexpr const char* ScanDirectionFlag = "Scan Direction Flag";
	//! Edge of flight line field name.
	constexpr const char* EdgeOfFlightLine = "EdgeOfFlightLine";
	//! Classification field name.
	constexpr const char* Classification = "Classification";
	//! Synthetic flag field name.
	constexpr const char* SyntheticFlag = "Synthetic Flag";
	//! Keypoint flag field name.
	constexpr const char* KeypointFlag = "Keypoint Flag";
	//! Withheld flag field name.
	constexpr const char* WithheldFlag = "Withheld Flag";
	//! Scan angle rank field name.
	constexpr const char* ScanAngleRank = "Scan Angle Rank";
	//! User data field name.
	constexpr const char* UserData = "User Data";
	//! Point source ID field name.
	constexpr const char* PointSourceId = "Point Source ID";
	//! GPS time field name.
	constexpr const char* GpsTime = "Gps Time";

	// 1.4 point format 6 stuff
	//! Scan angle field name.
	constexpr const char* ScanAngle = "Scan Angle";
	//! Scanner channel field name.
	constexpr const char* ScannerChannel = "Scanner Channel";
	//! Overlap flag field name.
	constexpr const char* OverlapFlag = "Overlap Flag";
	//! Near infrared field name.
	constexpr const char* NearInfrared = "Near Infrared";
} // namespace LasNames

/**
 * @brief LAS format details namespace.
 */
namespace LasDetails
{
	/// @brief Unscaled extent type.
	/// @details Alias for double-precision bounding box:
	/// - LAS file extent (from header)
	/// - COPC file extent (from copc::Info)
	/// @note COPC extent is cubic (octree shape)
	using UnscaledExtent = CCCoreLib::BoundingBoxTpl<double>;

	/**
	 * @brief LAZ chunk interval with file offset.
	 *
	 * Used for COPC reading. Includes point offset for seeking
	 * since LASzip API doesn't support byte seeking.
	 */
	struct ChunkInterval
	{
		/**
		 * @brief Filter status enumeration.
		 */
		enum class eFilterStatus
		{
			PASS         = 0, //!< Load chunk.
			INTERSECT_BB = 1, //!< Check intersection.
			FAIL         = 2  //!< Skip chunk.
		};

		//! Default constructor.
		ChunkInterval() = default;

		/**
		 * @brief Create chunk interval.
		 *
		 * @param[in] _pointOffsetInFile Offset in file.
		 * @param[in] _pointCount Number of points.
		 */
		ChunkInterval(uint64_t _pointOffsetInFile, uint64_t _pointCount)
		    : pointOffsetInFile(_pointOffsetInFile)
		    , pointCount(_pointCount)
		{
		}

		//! Offset in LAS file.
		uint64_t pointOffsetInFile{0};

		//! Offset in CC cloud (LOD construction).
		uint64_t pointOffsetInCCCloud{0};

		//! Point count in chunk.
		uint64_t pointCount{0};

		//! Filtered point count.
		uint64_t filteredPointCount{0};

		//! Filter status.
		eFilterStatus status{eFilterStatus::PASS};
	};

	/**
	 * @brief Get true point count from header.
	 *
	 * @param[in] laszipHeader LASzip header.
	 *
	 * @return True number of points.
	 */
	uint64_t TrueNumberOfPoints(const laszip_header* laszipHeader);

	//! Overlap flag bit position (fmt >= 6).
	constexpr unsigned OVERLAP_FLAG_BIT_POS  = 3;
	constexpr unsigned OVERLAP_FLAG_BIT_MASK = 1 << OVERLAP_FLAG_BIT_POS;

	/**
	 * @brief Extended VLR header for LAS files.
	 *
	 * EVLRs are stored after points in LAS files.
	 * Used for waveform data storage.
	 */
	struct EvlrHeader
	{
		static constexpr size_t SIZE             = 60; //!< Header size.
		static constexpr size_t USER_ID_SIZE     = 16; //!< User ID size.
		static constexpr size_t DESCRIPTION_SIZE = 32; //!< Description size.

		//! User ID.
		char userID[USER_ID_SIZE];

		//! Record ID.
		uint16_t recordID{0};

		//! Record length.
		uint64_t recordLength{0};

		//! Description.
		char description[DESCRIPTION_SIZE];

		//! Default constructor.
		EvlrHeader() = default;

		/**
		 * @brief Create waveform EVLR header.
		 *
		 * @return Waveform header.
		 */
		static EvlrHeader Waveform();

		/**
		 * @brief Check if waveform data packets.
		 *
		 * @return true if waveform.
		 */
		bool isWaveFormDataPackets() const;

		/**
		 * @brief Check if COPC entry.
		 *
		 * @return true if COPC.
		 */
		bool isCOPCEntry() const;

		//! Serialization operators.
		friend QDataStream& operator>>(QDataStream& stream, EvlrHeader& hdr);
		friend QDataStream& operator<<(QDataStream& stream, const EvlrHeader& hdr);
	};

	/**
	 * @brief Get point format size.
	 *
	 * @param[in] pointFormat Point format ID.
	 *
	 * @return Size in bytes, 0 if unsupported.
	 */
	uint16_t PointFormatSize(unsigned pointFormat);

	/**
	 * @brief Get header size for version.
	 *
	 * @param[in] versionMinor Version minor.
	 *
	 * @return Header size.
	 */
	uint16_t HeaderSize(unsigned versionMinor);

	/**
	 * @brief Check if format supports GPS time.
	 *
	 * @param[in] pointFormatId Point format.
	 *
	 * @return true if supported.
	 */
	inline bool HasGpsTime(unsigned pointFormatId)
	{
		return pointFormatId == 1
		       || pointFormatId >= 3;
	}

	/**
	 * @brief Check if format supports RGB.
	 *
	 * @param[in] pointFormatId Point format.
	 *
	 * @return true if supported.
	 */
	inline bool HasRGB(unsigned pointFormatId)
	{
		return pointFormatId == 2
		       || pointFormatId == 3
		       || pointFormatId == 5
		       || pointFormatId == 7
		       || pointFormatId == 8
		       || pointFormatId == 10;
	}

	/**
	 * @brief Check if format supports waveforms.
	 *
	 * @param[in] pointFormatId Point format.
	 *
	 * @return true if supported.
	 */
	inline bool HasWaveform(unsigned pointFormatId)
	{
		return pointFormatId == 4
		       || pointFormatId == 5
		       || pointFormatId >= 9;
	}

	/**
	 * @brief Check if format supports NIR.
	 *
	 * @param[in] pointFormatId Point format.
	 *
	 * @return true if supported.
	 */
	inline bool HasNearInfrared(unsigned pointFormatId)
	{
		return pointFormatId == 8
		       || pointFormatId == 10;
	}

	/**
	 * @brief Get VLRs total size.
	 *
	 * @param[in] vlrs VLR array.
	 * @param[in] numVlrs Number of VLRs.
	 *
	 * @return Total bytes.
	 */
	unsigned SizeOfVlrs(const laszip_vlr_struct* vlrs, unsigned numVlrs);

	/**
	 * @brief Check if VLR is LASzip compression.
	 *
	 * @param[in] vlr VLR.
	 *
	 * @return true if LASzip.
	 */
	bool IsLaszipVlr(const laszip_vlr_struct&);

	/**
	 * @brief Check if VLR is extra bytes.
	 *
	 * @param[in] vlr VLR.
	 *
	 * @return true if extra bytes.
	 */
	bool IsExtraBytesVlr(const laszip_vlr_struct&);

	/**
	 * @brief Get available point formats.
	 *
	 * @param[in] version Version string (e.g., "1.2").
	 *
	 * @return Point format list.
	 */
	const std::vector<unsigned>& PointFormatsAvailableForVersion(const QString& version);

	/**
	 * @brief Get available versions.
	 *
	 * @return Version array.
	 */
	const std::array<const char*, 3>& AvailableVersions();

	/**
	 * @brief LAS version selection.
	 */
	struct LasVersion
	{
		int pointFormat  = 3; //!< Point format.
		int minorVersion = 2; //!< Version minor.
	};

	/**
	 * @brief Select best version for cloud.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] previousMinorVersion Previous version.
	 *
	 * @return Best version.
	 */
	LasVersion SelectBestVersion(const ccPointCloud& cloud, int previousMinorVersion = 0);

	/**
	 * @brief Clone VLR content.
	 *
	 * @param[in] src Source VLR.
	 * @param[out] dst Destination VLR.
	 */
	void CloneVlrInto(const laszip_vlr_struct& src, laszip_vlr_struct& dst);

} // namespace LasDetails
