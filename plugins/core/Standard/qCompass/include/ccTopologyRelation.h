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

#ifndef CC_TOPOLOGY_HEADER
#define CC_TOPOLOGY_HEADER

/**
 * @file ccTopologyRelation.h
 *
 * @brief Topology relation class
 *
 * Defines topological relationships between GeoObjects.
 */

#include "ccPointPair.h"

#include <ccPointCloud.h>

class ccGeoObject;

/**
 * @class ccTopologyRelation
 *
 * @brief Topology relation
 *
 * Defines topological/timing relationships between GeoObjects.
 */
class ccTopologyRelation : public ccPointPair
{
public:
	/**
	 * @brief Create topology relation
	 * @param[in] associatedCloud Associated cloud
	 * @param[in] older_id ID of older object
	 * @param[in] younger_id ID of younger object
	 * @param[in] type Relationship type
	 */
	ccTopologyRelation(ccPointCloud* associatedCloud, int older_id, int younger_id, int type);
	
	/**
	 * @brief Create from polyline
	 * @param[in] obj Source polyline
	 */
	ccTopologyRelation(ccPolyline* obj);

	/**
	 * @brief Build visible graphic representation
	 * @param[in] older Older object
	 * @param[in] younger Younger object
	 */
	void constructGraphic(ccGeoObject* older, ccGeoObject* younger);

	/// Update metadata
	void updateMetadata() override;

	/// Get younger object ID
	int getYoungerID();

	/// Get older object ID
	int getOlderID();
	
	/// Get relationship type
	int getType();

	/// Set relationship type
	void setType(int topologyType);

	/**
	 * @brief Check if object is topology relation
	 * @param[in] obj Object to check
	 * @return True if topology relation
	 */
	static bool isTopologyRelation(ccHObject* obj);

	/// Invert relationship type
	static int invertType(int type);

private:
	int m_older_id = -1;
	int m_younger_id = -1;
	int m_type;

public:
	/// X-cutting relationships
	static const int YOUNGER_THAN = 2;
	static const int OLDER_THAN = 4;

	/// Conformable relationships
	static const int IMMEDIATELY_FOLLOWS = 8 | YOUNGER_THAN;
	static const int IMMEDIATELY_PRECEDES = 16 | OLDER_THAN;

	/// Equivalence
	static const int EQUIVALENCE = 32;

	/// Other relationships
	static const int NOT_OLDER_THAN = 64;
	static const int NOT_YOUNGER_THAN = 128;
