/**
 * @file LasVlr.h
 *
 * @brief LAS Variable Length Record structure.
 *
 * @details Represents VLR data and extra scalar fields from LAS files.
 *
 * ## VLR Usage
 *
 * Variable Length Records store:
 * - Georeferencing information
 * - Projection/coordinate system
 * - LASzip compression metadata
 * - Extra bytes definitions
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

// LASzip
#include <laszip/laszip_api.h>
// Qt
#include <QDataStream>
#include <QMetaType>
#include <QString>

/**
 * @brief LAS Variable Length Record structure.
 *
 * Contains VLR data and extra scalar fields from LAS files.
 */
struct LasVlr
{
	//! Default constructor.
	LasVlr() = default;

	/**
	 * @brief Create from LAS header.
	 *
	 * @param[in] header LAS header.
	 */
	explicit LasVlr(const laszip_header& header);

	/**
	 * @brief Copy constructor.
	 *
	 * @param[in] rhs Source VLR.
	 */
	LasVlr(const LasVlr& rhs);

	/**
	 * @brief Copy assignment.
	 *
	 * @param[in] rhs Source VLR.
	 *
	 * @return Reference to this.
	 */
	LasVlr& operator=(LasVlr rhs);

	/**
	 * @brief Swap two VLRs.
	 *
	 * @param[in,out] lhs Left VLR.
	 * @param[in,out] rhs Right VLR.
	 */
	static void Swap(LasVlr& lhs, LasVlr& rhs) noexcept;

	/**
	 * @brief Convert to string.
	 *
	 * @return String representation.
	 */
	inline QString toString() const
	{
		return QString("VLRs: %1").arg(vlrs.size());
	}

	/**
	 * @brief Get VLR count.
	 *
	 * @return Number of VLRs.
	 */
	inline laszip_U32 numVlrs() const
	{
		return static_cast<laszip_U32>(vlrs.size());
	}

	/**
	 * @brief Serialization operator.
	 *
	 * @param[out] arch Archive.
	 * @param[in] object Object to serialize.
	 *
	 * @return Archive reference.
	 */
	friend QDataStream& operator<<(QDataStream& arch, const LasVlr& object)
	{
		arch << static_cast<quint64>(object.vlrs.size());
		for (const laszip_vlr_struct& v : object.vlrs)
		{
			arch << v.reserved;
			arch.writeRawData(v.user_id, 16 * sizeof(laszip_CHAR));
			arch << v.record_id;
			arch << v.record_length_after_header;
			arch.writeRawData(v.description, 32 * sizeof(laszip_CHAR));
			arch.writeRawData((const char*)v.data, v.record_length_after_header);
		}

		arch << static_cast<quint64>(object.extraScalarFields.size());
		for (const LasExtraScalarField& e : object.extraScalarFields)
		{
			arch.writeRawData((const char*)&e, sizeof(LasExtraScalarField));
		}
		return arch;
	}

	/**
	 * @brief Deserialization operator.
	 *
	 * @param[in] arch Archive.
	 * @param[out] object Object to deserialize.
	 *
	 * @return Archive reference.
	 */
	friend QDataStream& operator>>(QDataStream& arch, LasVlr& object)
	{
		quint64 vlrSize = 0;
		arch >> vlrSize;
		object.vlrs.reserve(vlrSize);
		for (quint64 i = 0; i < vlrSize; ++i)
		{
			laszip_vlr_struct v;
			arch >> v.reserved;
			arch.readRawData((char*)v.user_id, 16 * sizeof(laszip_CHAR));
			arch >> v.record_id;
			arch >> v.record_length_after_header;
			arch.readRawData((char*)v.description, 32 * sizeof(laszip_CHAR));
			{
				v.data = new laszip_U8[v.record_length_after_header];
				arch.readRawData((char*)v.data, v.record_length_after_header);
			}
			object.vlrs.push_back(v);
		}

		quint64 extraScalarFieldCount = 0;
		arch >> extraScalarFieldCount;
		object.extraScalarFields.reserve(extraScalarFieldCount);
		for (quint64 i = 0; i < extraScalarFieldCount; ++i)
		{
			LasExtraScalarField e;
			{
				char* data = (char*)&e;
				uint  len  = sizeof(LasExtraScalarField);
				arch.readRawData(data, len);
			}
			object.extraScalarFields.push_back(e);
		}

		return arch;
	}

	//! VLR structures.
	std::vector<laszip_vlr_struct> vlrs;

	//! Extra scalar fields.
	std::vector<LasExtraScalarField> extraScalarFields;
};

Q_DECLARE_METATYPE(LasVlr);
