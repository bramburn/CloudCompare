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

#ifndef CC_POINTPAIR_HEADER
#define CC_POINTPAIR_HEADER

/**
 * @file ccPointPair.h
 *
 * @brief Point pair base class
 *
 * Base class for measurements comprising point pairs.
 */

#include <ccPolyline.h>
#include <ccSphere.h>
#include <ccCylinder.h>
#include <ccCone.h>
#include <GenericIndexedCloudPersist.h>
#include <ccPointCloud.h>

#include "ccMeasurement.h"

/**
 * @class ccPointPair
 *
 * @brief Point pair base class
 *
 * Base class for measurements comprising individual or pairs of points.
 */
class ccPointPair : 
	public ccPolyline, 
	public ccMeasurement
{
public:
	/**
	 * @brief Create point pair
	 * @param[in] associatedCloud Associated point cloud
	 */
	ccPointPair(ccPointCloud* associatedCloud);
	
	/**
	 * @brief Create from polyline
	 * @param[in] obj Source polyline
	 */
	ccPointPair(ccPolyline* obj);

	/// Destructor
	virtual ~ccPointPair() {}

	/// Update metadata
	virtual void updateMetadata() { };

	/// Get direction (not normalized)
	CCVector3 getDirection();

protected:
	/// Marker scale for drawing
	float m_relMarkerScale = 5.0f;

	/// Draw object
	virtual void drawMeOnly(CC_DRAW_CONTEXT& context) override;

public:
	/**
	 * @brief Check if object is a point pair
	 * @param[in] object Object to check
	 * @return True if point pair
	 */
	static bool isPointPair(ccHObject* object);
};

#endif
