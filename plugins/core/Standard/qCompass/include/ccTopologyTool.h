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

#ifndef CC_TOPOLOGYTOOL_HEADER
#define CC_TOPOLOGYTOOL_HEADER

/**
 * @file ccTopologyTool.h
 *
 * @brief Topology tool
 *
 * Tool for assigning topology relationships between GeoObjects.
 */

#include "ccTool.h"
#include "ccGeoObject.h"
#include "ccTopologyRelation.h"

#include <ccColorTypes.h>
#include <DistanceComputationTools.h>

/**
 * @class ccTopologyTool
 *
 * @brief Topology tool
 *
 * Tool for assigning timing relationships between GeoObjects.
 */
class ccTopologyTool :
	public ccTool
{
public:
	/// Constructor
	ccTopologyTool();
	
	/// Destructor
	virtual ~ccTopologyTool() = default;

	/// Tool activated
	virtual void toolActivated() override;

	/// Tool disactivated
	virtual void toolDisactivated() override;

	/// Selection changed callback
	virtual void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/// Accept action
	void accept() override;

	/// Cancel action
	void cancel() override;

protected:
	/// First object of topology pair
	int m_firstPick = -1;

public:
	/// Relationship type being assigned
	static int RELATIONSHIP;
};

#endif
