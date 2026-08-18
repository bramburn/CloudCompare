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

#ifndef CC_NORMAL_COMPRESSOR_HEADER
#define CC_NORMAL_COMPRESSOR_HEADER

/**
 * @file ccNormalCompressor.h
 *
 * @brief Normal vector compression/decompression
 *
 * Provides efficient compression of 3D normal vectors using
 * spherical quantization. Supports 2M+ unique normal directions.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccBasicTypes.h"
#include "qCC_db.h"

/**
 * @brief Normal vector compressor
 *
 * Compresses/decompresses 3D normal vectors using
 * spherical quantization.
 */
class QCC_DB_LIB_API ccNormalCompressor
{
  public:
	//! Quantization level (2^19 = 524288 directions)
	static const unsigned char QUANTIZE_LEVEL = 9;

	//! Maximum valid normal code
	static const unsigned MAX_VALID_NORM_CODE = (1 << (QUANTIZE_LEVEL * 2 + 3)) - 1;
	//! Null normal code (invalid)
	static const unsigned NULL_NORM_CODE = MAX_VALID_NORM_CODE + 1;

	/**
	 * @brief Compress a normal vector
	 * @param[in] N Normal components [Nx, Ny, Nz]
	 * @return Compressed normal code
	 */
	static unsigned Compress(const PointCoordinateType N[3]);

	/**
	 * @brief Decompress a normal code
	 * @param[in] index Compressed normal code
	 * @param[out] N Normal components [Nx, Ny, Nz]
	 * @param[in] level Quantization level
	 */
	static void Decompress(unsigned index, PointCoordinateType N[3], unsigned char level = QUANTIZE_LEVEL);

	/**
	 * @brief Invert a compressed normal
	 * @param[in,out] code Normal code to invert
	 */
	static void InvertNormal(CompressedNormType& code);
};

#endif // CC_NORMAL_COMPRESSOR_HEADER
