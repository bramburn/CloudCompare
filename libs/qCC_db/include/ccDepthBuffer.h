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

#ifndef CC_DEPTH_BUFFER_HEADER
#define CC_DEPTH_BUFFER_HEADER

// local
#include "qCC_db.h"

#include <CCGeom.h>

/**
 * @file ccDepthBuffer.h
 *
 * @brief Depth buffer (depth map) class
 *
 * Represents a depth map from a range sensor. Contains depth values
 * along with scan parameters (pitch/yaw steps) for projecting
 * back to 3D coordinates.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// System
#include <vector>

/**
 * @brief Depth buffer (depth map)
 *
 * Stores depth values from a range sensor with scan parameters.
 * Used for terrestrial LiDAR depth maps.
 */
class QCC_DB_LIB_API ccDepthBuffer
{
  public:
	/**
	 * @brief Default constructor
	 */
	ccDepthBuffer();
	/**
	 * @brief Destructor
	 */
	virtual ~ccDepthBuffer();

	//! Z-buffer grid (depth values)
	std::vector<PointCoordinateType> zBuff;
	//! Pitch step (radians)
	PointCoordinateType deltaPhi;
	//! Yaw step (radians)
	PointCoordinateType deltaTheta;
	//! Buffer width (columns)
	unsigned width;
	//! Buffer height (rows)
	unsigned height;

	/**
	 * @brief Clear the buffer
	 */
	void clear();

	/**
	 * @brief Fill small holes in the depth map
	 *
	 * Applies a mean filter to fill small gaps.
	 * @return 0 on success, negative on error
	 */
	int fillHoles();
};

#endif // CC_DEPTH_BUFFER_HEADER
