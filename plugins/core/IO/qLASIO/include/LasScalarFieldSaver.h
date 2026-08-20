/**
 * @file LasScalarFieldSaver.h
 *
 * @brief LAS scalar field saver.
 *
 * @details Saves CloudCompare scalar fields to LAS files.
 *
 * ## Saving Process
 *
 * 1. Create saver with field mappings
 * 2. For each point, call handleScalarFields()
 * 3. Scalar values are written to LAS format
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

struct laszip_point;

/**
 * @brief LAS scalar field saver.
 *
 * Saves CloudCompare scalar fields to LAS files.
 */
class LasScalarFieldSaver
{
  public:
	/**
	 * @brief Default constructor.
	 */
	LasScalarFieldSaver() = default;

	/**
	 * @brief Create saver with fields.
	 *
	 * @param[in] standardFields Standard field mappings.
	 * @param[in] extraFields Extra field mappings.
	 */
	LasScalarFieldSaver(std::vector<LasScalarField>&&      standardFields,
	                    std::vector<LasExtraScalarField>&& extraFields);

	/**
	 * @brief Set standard fields.
	 *
	 * @param[in] standardFields Standard field mappings.
	 */
	inline void setStandarFields(std::vector<LasScalarField>&& standardFields)
	{
		m_standardFields = standardFields;
		for (const LasScalarField& field : m_standardFields)
		{
			if (strcmp(field.name(), LasNames::Classification) == 0
			    && field.sf
			    && field.sf->getMax() >= 32)
			{
				m_classificationWasDecomposed = false;
			}
		}
	}

	/**
	 * @brief Set extra fields.
	 *
	 * @param[in] extraFields Extra field mappings.
	 */
	inline void setExtraFields(std::vector<LasExtraScalarField>&& extraFields)
	{
		m_extraFields = extraFields;
	}

	/**
	 * @brief Get extra fields.
	 *
	 * @return Extra fields.
	 */
	inline const std::vector<LasExtraScalarField>& extraFields() const
	{
		return m_extraFields;
	}

	/**
	 * @brief Handle scalar fields for point.
	 *
	 * @param[in] pointIndex Point index.
	 * @param[out] point LAS point structure.
	 */
	void handleScalarFields(size_t pointIndex, laszip_point& point);

	/**
	 * @brief Handle extra fields for point.
	 *
	 * @param[in] pointIndex Point index.
	 * @param[out] point LAS point structure.
	 */
	void handleExtraFields(size_t pointIndex, laszip_point& point);

  private:
	/**
	 * @brief Write scalar value with type conversion.
	 *
	 * @tparam T Target type.
	 * @param[in] value Scalar value.
	 * @param[out] dest Destination buffer.
	 */
	template <typename T>
	static void WriteScalarValueAs(ScalarType value, uint8_t* dest)
	{
		if (value > static_cast<ScalarType>(std::numeric_limits<T>::max()))
		{
			*reinterpret_cast<T*>(dest) = std::numeric_limits<T>::max();
		}
		else if (value < static_cast<ScalarType>(std::numeric_limits<T>::lowest()))
		{
			*reinterpret_cast<T*>(dest) = std::numeric_limits<T>::lowest();
		}
		else
		{
			*reinterpret_cast<T*>(dest) = static_cast<T>(value);
		}
	}

  private:
	//! Standard fields.
	std::vector<LasScalarField> m_standardFields;

	//! Extra fields.
	std::vector<LasExtraScalarField> m_extraFields;

	//! Classification was decomposed.
	bool m_classificationWasDecomposed{false};
};
