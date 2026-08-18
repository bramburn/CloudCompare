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

#ifndef CC_NODE_HEADER
#define CC_NODE_HEADER

/**
 * @file ccPinchNode.h
 *
 * @brief Pinch node class
 *
 * Represents pinch nodes created with qCompass.
 */

#include "ccPointPair.h"

#include <ccPointCloud.h>

/**
 * @class ccPinchNode
 *
 * @brief Pinch node
 *
 * Represents/draws pinch nodes created with qCompass.
 */
class ccPinchNode : public ccPointPair
{
public:
	/**
	 * @brief Create pinch node
	 * @param[in] associatedCloud Associated point cloud
	 */
	ccPinchNode(ccPointCloud* associatedCloud);
	
	/**
	 * @brief Create from polyline
	 * @param[in] obj Source polyline
	 */
	ccPinchNode(ccPolyline* obj);

	/// Update metadata
	void updateMetadata() override;

	/**
	 * @brief Check if object is a pinch node
	 * @param[in] obj Object to check
	 * @return True if pinch node
	 */
	static bool isPinchNode(ccHObject* obj);
};

#endif
