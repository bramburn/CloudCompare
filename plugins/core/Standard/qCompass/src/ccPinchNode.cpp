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
 * @file ccPinchNode.cpp
 *
 * @brief Compass pinch node implementation
 *
 * A pinch node is a structural geology marker representing
 * a pinch-out or truncation point where a geological layer
 * terminates against another structure.
 *
 * Stored as a 3D point with a thickness value.
 *
 * @see ccPinchNode.h
 */
#include "ccPinchNode.h"

//pass ctors straight to PointPair
ccPinchNode::ccPinchNode(ccPointCloud* associatedCloud)
	: ccPointPair(associatedCloud)
{
	updateMetadata();
}

ccPinchNode::ccPinchNode(ccPolyline* obj)
	: ccPointPair(obj)
{ 
	updateMetadata();
}

void ccPinchNode::updateMetadata()
{
	//add metadata tag defining the ccCompass class type
	setMetaData("ccCompassType", "PinchNode");

	//set drawing stuff (not really metadata, but hey!)
	setDefaultColor(ccColor::blue);
	setActiveColor(ccColor::orange);
	setHighlightColor(ccColor::orange);
	setAlternateColor(ccColor::orange);
}

//returns true if object is a lineation
bool ccPinchNode::isPinchNode(ccHObject* object)
{
	if (object->hasMetaData("ccCompassType"))
	{
		return object->getMetaData("ccCompassType").toString().contains("PinchNode");
	}
	return false;
}
