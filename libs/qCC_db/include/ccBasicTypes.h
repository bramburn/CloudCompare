// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccBasicTypes.h
 *
 * @brief Basic CloudCompare type definitions.
 *
 * @details Provides type aliases and definitions used throughout
 * the CloudCompare codebase.
 *
 * ## Type Categories
 *
 * - **Vector types**: CCVector2, CCVector3, CCVector4
 * - **Matrix types**: ccGLMatrix, ccGLMatrixd
 * - **Compressed types**: Compressed normals, colors
 * - **Point cloud types**: ScalarType, ReferenceCloud
 *
 * ## Compressed Normals
 *
 * Normals are compressed as indices into a lookup table
 * for efficient storage and rendering.
 *
 * ## Related Headers
 *
 * - CCCoreLib/include/CCTypes.h - Core type definitions
 * - CCCoreLib/include/CCGeom.h - Geometric types
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_BASIC_TYPES_HEADER
#define CC_BASIC_TYPES_HEADER

// CCCoreLib
#include <CCTypes.h>

/**
 * @brief Compressed normal type.
 *
 * @details Normals are stored as indices into a compressed normal
 * table for efficient storage and GPU rendering.
 *
 * The normal table contains 162,450 entries covering the
 * sphere surface.
 *
 * @see CCCoreLib::NormsTableHolder
 */
using CompressedNormType = unsigned int;

#endif // CC_BASIC_TYPES_HEADER
