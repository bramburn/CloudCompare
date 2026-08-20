/**
 * @file LasScalarFieldLoader.h
 *
 * @brief LAS scalar field loader.
 *
 * @details Loads LAS dimensions into CloudCompare scalar fields.
 *
 * ## Loading Process
 *
 * 1. Create loader with field mappings
 * 2. For each point, call handleScalarFields()
 * 3. Scalar values are stored in CloudCompare scalar fields
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

#include "LasDetails.h"
#include "LasExtraScalarField.h"
#include "LasScalarField.h"

// qCC_db
#include <FileIOFilter.h>
#include <ccPointCloud.h>

// Qt
#include <QFileInfo>

// LASzip
#include <laszip/laszip_api.h>

/**
 * @brief LAS scalar field loader.
 *
 * Loads LAS dimensions into CloudCompare scalar fields.
 */
class LasScalarFieldLoader
{
  public:
	/**
	 * @brief Create loader.
	 *
	 * @param[in,out] standardScalarFields Standard field mappings.
	 * @param[in,out] extraScalarFields Extra field mappings.
	 * @param[in,out] pointCloud Point cloud.
	 */
	LasScalarFieldLoader(std::vector<LasScalarField>&      standardScalarFields,
	                    std::vector<LasExtraScalarField>& extraScalarFields,
	                    ccPointCloud&                     pointCloud);

	/**
	 * @brief Handle scalar fields for current point.
	 *
	 * @param[in,out] pointCloud Point cloud.
	 * @param[in] currentPoint Current LAS point.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR handleScalarFields(ccPointCloud& pointCloud, const laszip_point& currentPoint);

	/**
	 * @brief Parse extra scalar field.
	 *
	 * @param[in] extraField Extra field definition.
	 * @param[in] currentPoint Current LAS point.
	 * @param[out] outputValues Output values.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR parseExtraScalarField(const LasExtraScalarField& extraField, const laszip_point& currentPoint, ScalarType outputValues[3]);

	/**
	 * @brief Handle RGB values.
	 *
	 * @param[in,out] pointCloud Point cloud.
	 * @param[in] currentPoint Current LAS point.
	 *
	 * @return Error code.
	 *
	 * @note Converts LAS RGB to CloudCompare color.
	 */
	CC_FILE_ERROR handleRGBValue(ccPointCloud& pointCloud, const laszip_point& currentPoint);

	/**
	 * @brief Handle extra scalar fields.
	 *
	 * @param[in] currentPoint Current LAS point.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR handleExtraScalarFields(const laszip_point& currentPoint);

	/**
	 * @brief Set ignore fields with default values.
	 *
	 * @param[in] state Ignore flag.
	 */
	inline void setIgnoreFieldsWithDefaultValues(bool state)
	{
		m_ignoreFieldsWithDefaultValues = state;
	}

	/**
	 * @brief Set force 8-bit RGB mode.
	 *
	 * @param[in] state Force flag.
	 */
	inline void setForce8bitRgbMode(bool state)
	{
		m_force8bitRgbMode = state;
	}

	/**
	 * @brief Set decompose classification.
	 *
	 * @param[in] state Decompose flag.
	 *
	 * @note Only for point format <= 5.
	 */
	inline void setDecomposeClassification(bool state)
	{
		m_decomposeClassification = state;
	}

	/**
	 * @brief Get standard fields.
	 *
	 * @return Standard scalar fields.
	 */
	inline const std::vector<LasScalarField>& standardFields() const
	{
		return m_standardFields;
	}

	/**
	 * @brief Get extra fields.
	 *
	 * @return Extra scalar fields.
	 */
	inline const std::vector<LasExtraScalarField>& extraFields() const
	{
		return m_extraScalarFields;
	}

  private:
	/**
	 * @brief Handle single scalar field.
	 *
	 * @tparam T Value type.
	 * @param[in,out] sfInfo Scalar field info.
	 * @param[in,out] pointCloud Point cloud.
	 * @param[in] currentValue Current value.
	 *
	 * @return Error code.
	 */
	template <typename T>
	CC_FILE_ERROR handleScalarField(LasScalarField& sfInfo, ccPointCloud& pointCloud, T currentValue);

	/**
	 * @brief Create scalar fields for extra bytes.
	 *
	 * @param[in,out] pointCloud Point cloud.
	 *
	 * @return true if successful.
	 */
	bool createScalarFieldsForExtraBytes(ccPointCloud& pointCloud);

	/**
	 * @brief Parse value of type.
	 *
	 * @tparam T Source type.
	 * @param[in] source Source bytes.
	 *
	 * @return Parsed value as ScalarType.
	 */
	template <typename T>
	static ScalarType ParseValueOfType(uint8_t* source);

	/**
	 * @brief Parse and convert value type.
	 *
	 * @tparam T Source type.
	 * @tparam V Destination type.
	 * @param[in] source Source bytes.
	 *
	 * @return Converted value.
	 */
	template <typename T, typename V>
	static V ParseValueOfTypeAs(const uint8_t* source);

	/**
	 * @brief Parse raw values from point.
	 *
	 * @param[in] extraField Extra field.
	 * @param[in] dataStart Data start pointer.
	 */
	void parseRawValues(const LasExtraScalarField& extraField, const uint8_t* dataStart);

	/**
	 * @brief Handle options for extra field.
	 *
	 * @tparam T Value type.
	 * @param[in] extraField Extra field.
	 * @param[in] inputValues Input values.
	 * @param[out] outputValues Output values.
	 */
	template <typename T>
	void handleOptionsFor(const LasExtraScalarField& extraField, T inputValues[3], ScalarType outputValues[3]);

  private:
	//! Force 8-bit RGB.
	bool m_force8bitRgbMode{false};

	//! Decompose classification.
	bool m_decomposeClassification{true};

	//! Ignore default values.
	bool m_ignoreFieldsWithDefaultValues{true};

	//! Color component shift.
	unsigned char m_colorCompShift{0};

	//! Standard fields.
	std::vector<LasScalarField>& m_standardFields;

	//! Extra fields.
	std::vector<LasExtraScalarField>& m_extraScalarFields;

	//! Raw values union.
	union
	{
		uint64_t unsignedValues[LasExtraScalarField::MAX_DIM_SIZE];
		int64_t  signedValues[LasExtraScalarField::MAX_DIM_SIZE];
		double   floatingValues[LasExtraScalarField::MAX_DIM_SIZE];
	} m_rawValues{};
};
