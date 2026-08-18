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

#ifndef CC_THICKNESS_HEADER
#define CC_THICKNESS_HEADER

/**
 * @file ccThickness.h
 *
 * @brief Thickness measurement class
 *
 * Represents thickness measurements.
 */

#include "ccPointPair.h"

#include <ccPointCloud.h>

/**
 * @class ccThickness
 *
 * @brief Thickness measurement
 *
 * Represents thickness measurements.
 */
class ccThickness : public ccPointPair
{
public:
	/**
	 * @brief Create thickness measurement
	 * @param[in] associatedCloud Associated point cloud
	 */
	ccThickness(ccPointCloud* associatedCloud);
	
	/**
	 * @brief Create from polyline
	 * @param[in] obj Source polyline
	 */
	ccThickness(ccPolyline* obj);

	/// Update metadata
	void updateMetadata() override;

	/**
	 * @brief Check if object is a thickness measurement
	 * @param[in] obj Object to check
	 * @return True if thickness
	 */
	static bool isThickness(ccHObject* obj);
};

#endif
