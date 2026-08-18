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

#ifndef CC_LINEATION_HEADER
#define CC_LINEATION_HEADER

/**
 * @file ccLineation.h
 *
 * @brief Lineation class
 *
 * Represents lineations measured with compass.
 */

#include "ccPointPair.h"

#include <ccPointCloud.h>

/**
 * @class ccLineation
 *
 * @brief Lineation
 *
 * Represents/draws lineations measured with qCompass.
 */
class ccLineation : public ccPointPair
{
public:
	/**
	 * @brief Create lineation
	 * @param[in] associatedCloud Associated point cloud
	 */
	ccLineation(ccPointCloud* associatedCloud);
	
	/**
	 * @brief Create from polyline
	 * @param[in] obj Source polyline
	 */
	ccLineation(ccPolyline* obj);

	/// Update metadata
	void updateMetadata() override;

	/**
	 * @brief Check if object is a lineation
	 * @param[in] obj Object to check
	 * @return True if lineation
	 */
	static bool isLineation(ccHObject* obj);
};
#endif
