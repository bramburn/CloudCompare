/**
 * @file LasExtraScalarField.h
 *
 * @brief Extra scalar field support for LAS files.
 *
 * @details Represents extra bytes/extra dimensions in LAS 1.4+ files.
 *
 * ## Overview
 *
 * LAS 1.4 introduced "extra bytes" - custom per-point attributes.
 * LasExtraScalarField parses and manages these dimensions.
 *
 * ## Data Types
 *
 * - Unsigned integers (u8, u16, u32, u64)
 * - Signed integers (i8, i16, i32, i64)
 * - Floating point (f32, f64)
 *
 * ## Dimension Sizes
 *
 * Extra fields can be 1, 2, or 3 dimensional arrays.
 * e.g., RGB color would be a 3-element unsigned array.
 *
 * @author Thomas Montaigu
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.    #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        //
// ##########################################################################

#include <cstdint>
#include <string>
#include <vector>

class QDataStream;

class ccPointCloud;
class ccScalarField;

struct laszip_header;
struct laszip_vlr;
typedef laszip_vlr laszip_vlr_struct;

/**
 * @brief Extra scalar field in LAS files.
 *
 * Represents extra bytes dimensions in LAS 1.4+ files.
 */
class LasExtraScalarField
{
  public:
	//! Maximum array dimension size.
	static constexpr unsigned MAX_DIM_SIZE = 3;

	//! Maximum name length.
	static constexpr unsigned MAX_NAME_SIZE = 32;

	//! Maximum description length.
	static constexpr unsigned MAX_DESCRIPTION_SIZE = 32;

	/**
	 * @brief Dimension size enumeration.
	 */
	enum class DimensionSize
	{
		One   = 1, //!< Single value.
		Two   = 2, //!< 2D array.
		Three = 3  //!< 3D array (e.g., RGB).
	};

	/**
	 * @brief Data type enumeration.
	 */
	enum DataType
	{
		Undocumented = 0, //!< Undocumented type.
		u8           = 1, //!< Unsigned 8-bit.
		i8,               //!< Signed 8-bit.
		u16,              //!< Unsigned 16-bit.
		i16,              //!< Signed 16-bit.
		u32,              //!< Unsigned 32-bit.
		i32,              //!< Signed 32-bit.
		u64,              //!< Unsigned 64-bit.
		i64,              //!< Signed 64-bit.
		f32,              //!< Float 32-bit.
		f64,              //!< Float 64-bit.
		Invalid           //!< Invalid type.
	};

	/**
	 * @brief Data kind enumeration.
	 */
	enum Kind
	{
		Signed,   //!< Signed integer.
		Unsigned, //!< Unsigned integer.
		Floating  //!< Floating point.
	};

  public:
	//! Default constructor.
	LasExtraScalarField() = default;

	//! Serialization operators.
	friend QDataStream& operator>>(QDataStream& dataStream, LasExtraScalarField& extraScalarField);
	friend QDataStream& operator<<(QDataStream& dataStream, const LasExtraScalarField& extraScalarField);

  public: // static helper functions
	/**
	 * @brief Parse extra fields from header.
	 *
	 * @param[in] laszipHeader LAS header.
	 *
	 * @return Vector of extra scalar fields.
	 */
	static std::vector<LasExtraScalarField> ParseExtraScalarFields(const laszip_header& laszipHeader);

	/**
	 * @brief Parse extra fields from VLR.
	 *
	 * @param[in] extraBytesVlr Extra bytes VLR.
	 *
	 * @return Vector of extra scalar fields.
	 */
	static std::vector<LasExtraScalarField> ParseExtraScalarFields(const laszip_vlr_struct& extraBytesVlr);

	/**
	 * @brief Initialize VLR with extra fields.
	 *
	 * @param[out] vlr VLR to initialize.
	 * @param[in] extraFields Extra fields.
	 */
	static void InitExtraBytesVlr(laszip_vlr_struct& vlr, const std::vector<LasExtraScalarField>& extraFields);

	/**
	 * @brief Update byte offsets.
	 *
	 * @param[in,out] extraFields Extra fields.
	 */
	static void UpdateByteOffsets(std::vector<LasExtraScalarField>& extraFields);

	/**
	 * @brief Get total size of all extra bytes.
	 *
	 * @param[in] extraScalarFields Extra fields.
	 *
	 * @return Total byte size.
	 */
	static unsigned TotalExtraBytesSize(const std::vector<LasExtraScalarField>& extraScalarFields);

	/**
	 * @brief Match extra bytes to scalar fields.
	 *
	 * @param[in,out] extraScalarFields Extra fields.
	 * @param[in] pointCloud Point cloud.
	 */
	static void MatchExtraBytesToScalarFields(std::vector<LasExtraScalarField>& extraScalarFields,
	                                          const ccPointCloud&               pointCloud);

  public: // methods
	/**
	 * @brief Get type code.
	 *
	 * @return LAS spec type code.
	 */
	uint8_t typeCode() const;

	/**
	 * @brief Get element size.
	 *
	 * @return Bytes per element.
	 */
	unsigned elementSize() const;

	/**
	 * @brief Get number of dimensions.
	 *
	 * @return Dimension count.
	 */
	unsigned numElements() const;

	/**
	 * @brief Get byte size.
	 *
	 * @return Total bytes.
	 */
	unsigned byteSize() const;

	/**
	 * @brief Get data kind.
	 *
	 * @return Signed, Unsigned, or Floating.
	 */
	Kind kind() const;

	/**
	 * @brief Get type name.
	 *
	 * @return Type as string.
	 */
	std::string typeName() const;

	/**
	 * @brief Check if no-data is relevant.
	 */
	bool noDataIsRelevant() const;

	/**
	 * @brief Check if min is relevant.
	 */
	bool minIsRelevant() const;

	/**
	 * @brief Check if max is relevant.
	 */
	bool maxIsRelevant() const;

	/**
	 * @brief Check if scale is relevant.
	 */
	bool scaleIsRelevant() const;

	/**
	 * @brief Check if offset is relevant.
	 */
	bool offsetIsRelevant() const;

	/**
	 * @brief Set offset relevance.
	 */
	void setOffsetIsRelevant(bool isRelevant);

	/**
	 * @brief Set scale relevance.
	 */
	void setScaleIsRelevant(bool isRelevant);

	/**
	 * @brief Reset scalar field pointers.
	 */
	void resetScalarFieldsPointers();

	/**
	 * @brief Get data type from value.
	 *
	 * @param[in] value Type code value.
	 *
	 * @return Tuple of data type and dimension size.
	 */
	static std::tuple<DataType, DimensionSize> DataTypeFromValue(uint8_t value);

  public: // data members
	//! Data type.
	DataType type{Undocumented};

	//! Dimension size.
	DimensionSize dimensions{DimensionSize::One};

	//! Options flags.
	uint8_t options{0};

	//! Name (from VLR).
	char name[MAX_NAME_SIZE] = "";

	//! Description (from VLR).
	char description[MAX_DESCRIPTION_SIZE] = "";

	//! No-data values.
	uint8_t noData[MAX_DIM_SIZE][8] = {0};

	//! Min values.
	uint8_t mins[MAX_DIM_SIZE][8] = {0};

	//! Max values.
	uint8_t maxs[MAX_DIM_SIZE][8] = {0};

	//! Scale factors.
	double scales[MAX_DIM_SIZE] = {0.0};

	//! Offsets.
	double offsets[MAX_DIM_SIZE] = {0.0};

	//! Byte offset in record.
	unsigned byteOffset{0};

	//! Associated scalar fields.
	ccScalarField* scalarFields[MAX_DIM_SIZE] = {nullptr};

	//! CloudCompare name (may differ from original).
	char ccName[MAX_NAME_SIZE + 8] = {0};
};
