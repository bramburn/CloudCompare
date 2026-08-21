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
 * @file LasVlr.cpp
 *
 * @brief Variable Length Record container implementation
 *
 * VLRs (Variable Length Records) are extensible metadata blocks in
 * LAS files. They store arbitrary binary data (projection info,
 * coordinate systems, proprietary data, etc.).
 *
 * This file implements the LasVlr container class:
 * - VLRs are copied from the LAS header (excluding COPC/LAZzip/extra bytes)
 * - Serialized to/from QVariant for ccPointCloud metaData storage
 * - Extra scalar field definitions are stored alongside VLRs
 *
 * ## VLRs Excluded from Copy
 *
 * These are handled separately and not copied back:
 * - COPC VLRs (hierarchical octree index)
 * - LASzip VLRs (compression metadata)
 * - Extra bytes VLRs (replaced by structured LasExtraScalarField)
 *
 * @see LasVlr.h
 */

#include "LasVlr.h"

#include "CopcLoader.h"
#include "LasMetadata.h"

// Qt
#include <QtGlobal>
// qCC_db
#include <ccPointCloud.h>
// System
#include <algorithm>

/**
 * @brief Construct from a LAS header
 *
 * Copies all VLRs from the header, filtering out:
 * - COPC VLRs (managed by the octree index)
 * - LASzip VLRs (compression state)
 * - Extra bytes VLRs (replaced by LasExtraScalarField)
 *
 * @param[in] header Source LAS header
 */
LasVlr::LasVlr(const laszip_header& header)
{
	const auto vlrShouldBeCopied = [](const laszip_vlr_struct& vlr)
	{
		// Currently, we avoid copying back the COPC VLR (due to potential modifications to the file)
		return !LasDetails::IsLaszipVlr(vlr) && !LasDetails::IsExtraBytesVlr(vlr) && !copc::CopcLoader::IsCOPCVlr(vlr);
	};

	ptrdiff_t numVlrs = std::count_if(header.vlrs, header.vlrs + header.number_of_variable_length_records, vlrShouldBeCopied);
	if (numVlrs > 0)
	{
		vlrs.resize(numVlrs);
		laszip_U32 j = 0;
		for (laszip_U32 i = 0; i < header.number_of_variable_length_records; ++i)
		{
			if (vlrShouldBeCopied(header.vlrs[i]))
			{
				LasDetails::CloneVlrInto(header.vlrs[i], vlrs[j]);
				j++;
			}
		}
	}
}

LasVlr& LasVlr::operator=(LasVlr rhs)
{
	LasVlr::Swap(*this, rhs);
	return *this;
}

LasVlr::LasVlr(const LasVlr& rhs)
    : extraScalarFields(rhs.extraScalarFields)
{
	if (rhs.numVlrs() != 0)
	{
		vlrs.resize(rhs.numVlrs());
		for (laszip_U32 i = 0; i < rhs.numVlrs(); ++i)
		{
			LasDetails::CloneVlrInto(rhs.vlrs[i], vlrs[i]);
		}
	}
}

/**
 * @brief Swap two LasVlr instances (no-throw)
 *
 * @param[in,out] lhs First instance
 * @param[in,out] rhs Second instance
 */
void LasVlr::Swap(LasVlr& lhs, LasVlr& rhs) noexcept
{
	std::swap(lhs.vlrs, rhs.vlrs);
	std::swap(lhs.extraScalarFields, rhs.extraScalarFields);
}
