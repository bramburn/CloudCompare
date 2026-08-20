// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file LASFields.h
 *
 * @brief LAS file format field definitions.
 *
 * @details Defines LAS (LASer) file format field identifiers
 * and utilities for reading/writing LAS files.
 *
 * ## LAS Format
 *
 * LAS is a public binary file format for LIDAR data:
 * - Point coordinates (X, Y, Z)
 * - Intensity
 * - Return number
 * - Classification
 * - Colors (RGB)
 * - GPS time
 *
 * ## Point Formats
 *
 * LAS uses different point formats (0-10) depending on
 * what data is stored. Format 6+ is LAS 1.4+.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_LAS_FIELDS_HEADER
#define CC_LAS_FIELDS_HEADER

// Local
#include "qCC_io.h"

// qCC_db
#include <ccPointCloud.h>
#include <ccScalarField.h>

// Qt
#include <QSharedPointer>

// System
#include <vector>

class ccScalarField;
class ccPointCloud;

//! LAS scale X metadata key.
static const char LAS_SCALE_X_META_DATA[]         = "LAS.scale.x";
//! LAS scale Y metadata key.
static const char LAS_SCALE_Y_META_DATA[]         = "LAS.scale.y";
//! LAS scale Z metadata key.
static const char LAS_SCALE_Z_META_DATA[]         = "LAS.scale.z";
//! LAS offset X metadata key.
static const char LAS_OFFSET_X_META_DATA[]        = "LAS.offset.x";
//! LAS offset Y metadata key.
static const char LAS_OFFSET_Y_META_DATA[]        = "LAS.offset.y";
//! LAS offset Z metadata key.
static const char LAS_OFFSET_Z_META_DATA[]        = "LAS.offset.z";
//! LAS version major metadata key.
static const char LAS_VERSION_MAJOR_META_DATA[]   = "LAS.version.major";
//! LAS version minor metadata key.
static const char LAS_VERSION_MINOR_META_DATA[]   = "LAS.version.minor";
//! LAS point format metadata key.
static const char LAS_POINT_FORMAT_META_DATA[]    = "LAS.point_format";
//! LAS global encoding metadata key.
static const char LAS_GLOBAL_ENCODING_META_DATA[] = "LAS.global_encoding";
//! LAS project UUID metadata key.
static const char LAS_PROJECT_UUID_META_DATA[]    = "LAS.project_uuid";

/**
 * @brief LAS field identifiers.
 */
enum LAS_FIELDS
{
	LAS_X                 = 0, //!< X coordinate.
	LAS_Y                 = 1, //!< Y coordinate.
	LAS_Z                 = 2, //!< Z coordinate.
	LAS_INTENSITY         = 3, //!< Return intensity.
	LAS_RETURN_NUMBER     = 4, //!< Return number.
	LAS_NUMBER_OF_RETURNS = 5, //!< Number of returns.
	LAS_SCAN_DIRECTION    = 6, //!< Scan direction flag.
	LAS_FLIGHT_LINE_EDGE  = 7, //!< Edge of flight line.
	LAS_CLASSIFICATION    = 8, //!< Point classification.
	LAS_SCAN_ANGLE_RANK   = 9, //!< Scan angle rank.
	LAS_USER_DATA         = 10, //!< User data.
	LAS_POINT_SOURCE_ID   = 11, //!< Point source ID.
	LAS_RED               = 12, //!< Red color channel.
	LAS_GREEN             = 13, //!< Green color channel.
	LAS_BLUE              = 14, //!< Blue color channel.
	LAS_TIME              = 15, //!< GPS time.
	LAS_EXTRA             = 16, //!< Extra bytes.
	// Sub fields
	LAS_CLASSIF_VALUE     = 17, //!< Classification value.
	LAS_CLASSIF_SYNTHETIC = 18, //!< Synthetic flag.
	LAS_CLASSIF_KEYPOINT  = 19, //!< Key-point flag.
	LAS_CLASSIF_WITHHELD  = 20, //!< Withheld flag.
	LAS_CLASSIF_OVERLAP   = 21, //!< Overlap flag.
	// Invalid
	LAS_INVALID = 255 //!< Invalid field.
};

//! LAS field names.
const char LAS_FIELD_NAMES[][28] = {
    "X",
    "Y",
    "Z",
    "Intensity",
    "ReturnNumber",
    "NumberOfReturns",
    "ScanDirectionFlag",
    "EdgeOfFlightLine",
    "Classification",
    "ScanAngleRank",
    "UserData",
    "PointSourceId",
    "Red",
    "Green",
    "Blue",
    "GpsTime",
    "extra",
    "[Classif] Value",
    "[Classif] Synthetic flag",
    "[Classif] Key-point flag",
    "[Classif] Withheld flag",
    "[Classif] Overlap flag",
};

/**
 * @brief LAS field descriptor.
 */
struct LasField
{
	//! Shared pointer type.
	typedef QSharedPointer<LasField> Shared;

	/**
	 * @brief Default constructor.
	 *
	 * @param[in] fieldType LAS field type.
	 * @param[in] defaultVal Default value.
	 * @param[in] min Minimum valid value.
	 * @param[in] max Maximum valid value (-1 = no limit).
	 * @param[in] _minPointFormat Minimum point format.
	 */
	LasField(LAS_FIELDS fieldType = LAS_INVALID, double defaultVal = 0, double min = 0.0, double max = -1.0, uint8_t _minPointFormat = 0)
	    : type(fieldType)
	    , sf(nullptr)
	    , firstValue(0.0)
	    , minValue(min)
	    , maxValue(max)
	    , defaultValue(defaultVal)
	    , minPointFormat(_minPointFormat)
	{
	}

	/**
	 * @brief Get field name.
	 *
	 * @return Official LAS field name.
	 */
	QString getName() const
	{
		return type < LAS_INVALID ? QString(LAS_FIELD_NAMES[type]) : QString();
	}

	/**
	 * @brief Get LAS fields from point cloud.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[out] fieldsToSave Fields to save.
	 * @param[out] minPointFormat Minimum point format.
	 *
	 * @return true if successful.
	 */
	static bool GetLASFields(ccPointCloud* cloud, std::vector<LasField>& fieldsToSave, uint8_t& minPointFormat);

	/**
	 * @brief Get format record length.
	 *
	 * @param[in] pointFormat Point format version.
	 *
	 * @return Record length in bytes.
	 */
	static unsigned GetFormatRecordLength(uint8_t pointFormat);

	/**
	 * @brief Get version minor for point format.
	 *
	 * @param[in] pointFormat Point format.
	 *
	 * @return LAS version minor.
	 */
	static uint8_t VersionMinorForPointFormat(uint8_t pointFormat);

	/**
	 * @brief Update minimum point format.
	 *
	 * @param[in] minPointFormat Current minimum.
	 * @param[in] withRGB Has RGB data.
	 * @param[in] withFWF Has waveform data.
	 * @param[in] allowLegacyFormats Allow legacy formats.
	 *
	 * @return Updated minimum format.
	 */
	static uint8_t UpdateMinPointFormat(uint8_t minPointFormat, bool withRGB, bool withFWF, bool allowLegacyFormats = true);

	/**
	 * @brief Sanitize string for LAS.
	 *
	 * @param[in] str Input string.
	 *
	 * @return Sanitized string.
	 */
	static QString SanitizeString(const QString& str);

	/**
	 * @brief Desanitize string from LAS.
	 *
	 * @param[in] str LAS string.
	 *
	 * @return Desanitized string.
	 */
	static QString DesanitizeString(const QString& str);

	/**
	 * @brief Get safe value at index.
	 *
	 * @param[in] index Point index.
	 *
	 * @return Value or default.
	 */
	double getSafeValue(unsigned index) const
	{
		if (sf)
		{
			ScalarType value = sf->getValue(index);
			if (CCCoreLib::ScalarField::ValidValue(value))
			{
				return value;
			}
			return defaultValue;
		}
		assert(false);
		return defaultValue;
	}

	//! Field type.
	LAS_FIELDS type;

	//! Associated scalar field.
	ccScalarField* sf;

	//! First value.
	double firstValue;

	//! Minimum value.
	double minValue;

	//! Maximum value.
	double maxValue;

	//! Default value.
	double defaultValue;

	//! Minimum point format.
	uint8_t minPointFormat;
};

#endif // CC_LAS_FIELDS_HEADER
