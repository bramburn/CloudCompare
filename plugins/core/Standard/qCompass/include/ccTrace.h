#pragma once

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

/**
 * @file ccCompassTrace.h
 *
 * @brief Trace polyline class
 *
 * Polyline controlled by waypoints using least-cost path algorithm.
 */

//#define DEBUG_PATH

#include <ccHObject.h>
#include <ccPolyline.h>

#include "ccMeasurement.h"

#include <ccSphere.h>
#include <DgmOctree.h>
#include <DgmOctreeReferenceCloud.h>
#include <GenericIndexedCloudPersist.h>
#include <ccPointCloud.h>
#include <ccColorTypes.h>
#include <ccPlane.h>
#include <Jacobi.h>
#include <ccScalarField.h>
#include <ccProgressDialog.h>
#include <ScalarFieldTools.h>

#include "ccFitPlane.h"

#include <vector>
#include <algorithm>
#include <deque>

/**
 * @class ccCompassTrace
 *
 * @brief Trace polyline
 *
 * Polyline controlled by waypoints using least-cost path algorithm
 * for picking fracture traces and lithological contacts.
 */
class ccCompassTrace :
	public ccPolyline,
	public ccMeasurement
{
public:
	/**
	 * @brief Create trace
	 * @param[in] associatedCloud Associated point cloud
	 */
	ccCompassTrace(ccPointCloud* associatedCloud = nullptr);
	
	/**
	 * @brief Create from polyline
	 * @param[in] poly Source polyline
	 */
	ccCompassTrace(ccPolyline* poly);
	
	/// Destructor
	~ccCompassTrace() override {}

	/// Get class name
	static QString GetClassName() { return "CompassTrace"; }

	/// Get class ID
	inline CC_CLASS_ENUM getClassID() const override { return CC_TYPES::CUSTOM_H_OBJECT | CC_TYPES::POLY_LINE; }
	
	/// Handle deletion
	void onDeletionOf(const ccHObject* obj) override;

	/// Set associated cloud
	void setAssociatedCloud(GenericIndexedCloudPersist* cloud) override;
	
	/// Load from file
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;

	/**
	 * @brief Add waypoint
	 * @param[in] pointId Index of point to add as waypoint
	 */
	void pushWaypoint(int pointId) { m_waypoints.push_back(pointId); }
};
