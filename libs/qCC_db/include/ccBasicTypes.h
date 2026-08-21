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
 * @brief Basic CloudCompare type aliases
 *
 * Provides the primary type aliases used throughout the codebase.
 * Most are re-exported from CCCoreLib (CCTypes.h, CCGeom.h).
 *
 * Key type aliases:
 * - CompressedNormType: index into the normal directions lookup table
 *   (162,450 discrete normals on the sphere surface)
 *
 * Vector types (from CCCoreLib):
 * - CCVector2: 2D point/coordinate (float or double via template)
 * - CCVector3: 3D point/coordinate
 * - CCVector4: homogeneous 3D coordinate (x,y,z,w)
 *
 * ScalarType: float by default; the numeric type for all per-point
 * scalar fields (intensity, classification, curvature, etc.)
 *
 * PointCoordinateType: double by default; the numeric type for
 * point cloud coordinates.
 *
 * @see CCCoreLib/include/CCTypes.h for the core scalar/point type definitions
 * @see CCCoreLib/include/CCGeom.h for geometric types
 */

#ifndef CC_BASIC_TYPES_HEADER
#define CC_BASIC_TYPES_HEADER

// CCCoreLib
#include <CCTypes.h>

/**
 * @brief Compressed normal direction type
 *
 * Normals are stored as indices into the global normal directions
 * lookup table (ccNormalVectors) rather than as 3 floats.
 * The table contains 162,450 discrete directions on the sphere.
 *
 * Conversion:
 * - Compress: ccNormalVectors::GetNormIndex(n)
 * - Decompress: ccNormalVectors::GetNormal(idx)
 *
 * @see CCCoreLib::NormsTableHolder
 * @see ccNormalVectors for the lookup table
 */
using CompressedNormType = unsigned int;

#endif // CC_BASIC_TYPES_HEADER
