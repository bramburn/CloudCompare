/**
 * @file LasScalarField.h
 *
 * @brief LAS scalar field mapping structures.
 *
 * @details Links LAS standard fields to CloudCompare scalar fields
 * for reading and writing LAS files.
 *
 * ## Scalar Fields
 *
 * LAS standard scalar fields include:
 * - Intensity
 * - Return Number
 * - Classification
 * - GPS Time
 * - Scan Angle
 * - And more...
 *
 * ## Extended Fields (LAS 1.4+)
 *
 * Extended fields have higher precision and additional data.
 *
 * @author Thomas Montaigu
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        //
// ##########################################################################

// CCCoreLib
#include <CCTypes.h>
// qCC_db
#include <ccScalarField.h>

// System
#include <cstdint>

/**
 * @brief LAS scalar field mapping.
 *
 * Links a LAS standard field to a CloudCompare scalar field.
 */
struct LasScalarField
{
	/**
	 * @brief LAS field ID enumeration.
	 */
	enum Id
	{
		Intensity       = 0, //!< Return intensity.
		ReturnNumber    = 1, //!< Return number.
		NumberOfReturns = 2, //!< Number of returns.
		ScanDirectionFlag,   //!< Scan direction.
		EdgeOfFlightLine,    //!< Edge of flight line.
		Classification,      //!< Classification.
		SyntheticFlag,       //!< Synthetic flag.
		KeypointFlag,        //!< Keypoint flag.
		WithheldFlag,        //!< Withheld flag.
		ScanAngleRank,       //!< Scan angle rank.
		UserData,            //!< User data.
		PointSourceId,       //!< Point source ID.
		GpsTime,             //!< GPS time.
		// Extended (LAS 1.4)
		ExtendedScanAngle,       //!< Extended scan angle.
		ExtendedScannerChannel,  //!< Scanner channel.
		OverlapFlag,             //!< Overlap flag.
		ExtendedClassification,  //!< Extended classification.
		ExtendedReturnNumber,    //!< Extended return number.
		ExtendedNumberOfReturns, //!< Extended number of returns.
		NearInfrared             //!< Near infrared.
	};

	/**
	 * @brief Value range structure.
	 *
	 * Represents an inclusive range for detecting data loss.
	 */
	struct Range
	{
		/**
		 * @brief Create range from min/max.
		 *
		 * @tparam T Numeric type.
		 * @param[in] minValue Minimum value.
		 * @param[in] maxValue Maximum value.
		 */
		template <class T>
		constexpr Range(T minValue, T maxValue)
		    : min(static_cast<ScalarType>(minValue))
		    , max(static_cast<ScalarType>(maxValue))
		{
		}

		/**
		 * @brief Create range for type.
		 *
		 * @tparam T Numeric type.
		 *
		 * @return Range with type min/max.
		 */
		template <class T>
		static constexpr Range ForType()
		{
			return Range(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
		}

		/**
		 * @brief Create range for bit count.
		 *
		 * @param[in] numBits Number of bits.
		 *
		 * @return Range for bit count.
		 */
		static Range ForBitCount(uint8_t numBits)
		{
			return Range(0, ((1 << static_cast<uint32_t>(numBits)) - 1));
		}

		//! Minimum value.
		ScalarType min = 0;
		//! Maximum value.
		ScalarType max = 0;
	};

  public: // constructors
	//! Deleted default constructor.
	LasScalarField() = delete;

	/**
	 * @brief Create scalar field mapping.
	 *
	 * @param[in] id LAS field ID.
	 * @param[in] sf CloudCompare scalar field.
	 */
	explicit LasScalarField(LasScalarField::Id id, ccScalarField* sf = nullptr);

	/**
	 * @brief Get LAS field name.
	 *
	 * @return Field name.
	 */
	const char* name() const;

  public: // static functions
	/**
	 * @brief Get name from ID.
	 *
	 * @param[in] id Field ID.
	 *
	 * @return Field name.
	 */
	static constexpr const char* NameFromId(LasScalarField::Id id);

	/**
	 * @brief Get ID from name.
	 *
	 * @param[in] name Field name.
	 * @param[in] targetPointFormat Point format version.
	 *
	 * @return Field ID.
	 *
	 * @throws std::logic_error if not found.
	 */
	static LasScalarField::Id IdFromName(const char* name, unsigned targetPointFormat);

	/**
	 * @brief Get value range for field.
	 *
	 * @param[in] id Field ID.
	 *
	 * @return Value range.
	 */
	static LasScalarField::Range ValueRange(LasScalarField::Id id);

	/**
	 * @brief Get fields for point format.
	 *
	 * @param[in] pointFormatId Point format ID.
	 *
	 * @return Vector of scalar fields.
	 */
	static std::vector<LasScalarField> ForPointFormat(unsigned pointFormatId);

  public: // members
	//! LAS field ID.
	Id id;

	//! CloudCompare scalar field.
	//! - Reading: values stored here
	//! - Writing: values read from here
	ccScalarField* sf{nullptr};

	//! Value range.
	Range range;
};
