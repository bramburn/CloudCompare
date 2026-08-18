//##########################################################################
//#                                                                        #
//#                    CLOUDCOMPARE PLUGIN: ccCompass                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                     COPYRIGHT: Sam Thiele  2017                        #
//#                                                                        #
//##########################################################################

#ifndef CC_FITPLANE_HEADER
#define CC_FITPLANE_HEADER

/**
 * @file ccFitPlane.h
 *
 * @brief Fit plane class
 *
 * Plane of best fit for compass measurements.
 */

#include <ccPlane.h>
#include <ccNormalVectors.h>

#include "ccMeasurement.h"

/**
 * @class ccFitPlane
 *
 * @brief Fit plane
 *
 * Plane of best fit for geological measurements.
 */
class ccFitPlane :
	public ccPlane, 
	public ccMeasurement
{
public:
	/**
	 * @brief Create from plane
	 * @param[in] p Input plane
	 */
	ccFitPlane(ccPlane* p);
	
	/// Destructor
	~ccFitPlane() = default;

	/**
	 * @brief Update plane attributes
	 * @param[in] rms Root mean square error
	 * @param[in] search_r Search radius
	 */
	void updateAttributes(float rms, float search_r);

	/**
	 * @brief Fit plane to point cloud
	 * @param[in] cloud Point cloud
	 * @param[out] rms Root mean square error
	 * @return Fitted plane or nullptr
	 */
	static ccFitPlane* Fit(CCCoreLib::GenericIndexedCloudPersist* cloud, double *rms);

	/**
	 * @brief Check if object is a fit plane
	 * @param[in] object Object to check
	 * @return True if fit plane
	 */
	static bool isFitPlane(ccHObject* object);
};

#endif
