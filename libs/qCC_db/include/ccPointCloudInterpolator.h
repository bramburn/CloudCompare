#pragma once

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
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

/**
 * @file ccPointCloudInterpolator.h
 *
 * @brief Point cloud interpolation
 *
 * Scalar field interpolation between point clouds.
 *
 * @author Daniel Girardeau-Montaut
 */

// Local
#include "qCC_db.h"

// System
#include <vector>

namespace CCCoreLib
{
	class GenericProgressCallback;
}

/**
 * @brief Point cloud interpolator
 *
 * Interpolation utilities for point clouds.
 */
class QCC_DB_LIB_API ccPointCloudInterpolator
{
  public:
	/**
	 * @brief Interpolation parameters
	 */
	struct Parameters
	{
		/// Interpolation method
		enum Method
		{
			NEAREST_NEIGHBOR,
			K_NEAREST_NEIGHBORS,
			RADIUS
		};
		
		/// Interpolation algorithm
		enum Algo
		{
			AVERAGE,
			MEDIAN,
			NORMAL_DIST
		};

		Method   method          = NEAREST_NEIGHBOR;
		Algo     algo            = AVERAGE;
		unsigned knn             = 0;
		float    radius          = 0;
		double   sigma           = 0;
		bool     noNormalization = false;
	};

	/**
	 * @brief Interpolate scalar fields
	 * @param[in] destCloud Destination cloud
	 * @param[in] srcCloud Source cloud
	 * @param[in] sfIndexes Scalar field indexes
	 * @param[in] params Interpolation parameters
	 * @param[in] progressCb Progress callback
	 * @param[in] octreeLevel Octree level
	 * @return true on success
	 */
	static bool InterpolateScalarFieldsFrom(ccPointCloud*                       destCloud,
	                                        ccPointCloud*                       srcCloud,
	                                        const std::vector<int>&             sfIndexes,
	                                        const Parameters&                   params,
	                                        CCCoreLib::GenericProgressCallback* progressCb  = nullptr,
	                                        unsigned char                       octreeLevel = 0);
};
