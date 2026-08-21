// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_FLAGS_HEADER
#define CC_FLAGS_HEADER

/**
 * @file ccFlags.h
 *
 * @brief 8-bit bitfield container
 *
 * Represents up to 8 boolean flags as a bitfield, with conversion
 * to/from an unsigned char byte for serialization.
 *
 * Usage: subclasses or users define which bit position means what.
 * The class provides no semantics — just the storage and conversion.
 *
 * @see Used by ccPointCloud::reserveTheNormsTable / reserveTheColorsTable
 *      to track which of 8 possible normals/color channels are present.
 */

#include <cstring>

/**
 * @class ccFlags
 *
 * @brief 8-bit flags container with byte serialization
 *
 * Stores 8 boolean flags in a bit-like array. Provides fromByte() and
 * toByte() for converting to/from a single unsigned char for compact
 * file storage.
 *
 * Bits are uninterpreted — the caller defines the meaning of each
 * bit position.
 */
class ccFlags
{
  public:
	/**
	 * @brief Reset all 8 flags to false
	 */
	void reset()
	{
		memset(table, 0, sizeof(bool) * 8);
	}

	/**
	 * @brief Set flags from a byte value
	 *
	 * @param[in] byte Bitfield where bit i sets table[i]
	 */
	void fromByte(unsigned char byte)
	{
		unsigned char i, mask = 1;
		for (i = 0; i < 8; ++i)
		{
			table[i] = ((byte & mask) == mask);
			mask <<= 1;
		}
	}

	/**
	 * @brief Convert flags to a byte
	 *
	 * @return Byte where bit i is set if table[i] is true
	 */
	unsigned char toByte() const
	{
		unsigned char i, byte = 0, mask = 1;
		for (i = 0; i < 8; ++i)
		{
			if (table[i])
				byte |= mask;
			mask <<= 1;
		}
		return byte;
	}

	//! Flag bits (index = bit position 0-7)
	bool table[8];
};

#endif // CC_FLAGS_HEADER
