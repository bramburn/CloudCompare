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

#ifndef CC_NODETOOL_HEADER
#define CC_NODETOOL_HEADER

/**
 * @file ccPinchNodeTool.h
 *
 * @brief Pinch node tool
 *
 * Tool for creating pinch nodes.
 */

#include "ccTool.h"
#include "ccGeoObject.h"
#include "ccPinchNode.h"

#include <qinputdialog.h>
#include <qmainwindow.h>

/**
 * @class ccPinchNodeTool
 *
 * @brief Pinch node tool
 *
 * Tool for creating pinch nodes.
 */
class ccPinchNodeTool :	public ccTool
{
public:
	/// Constructor
	ccPinchNodeTool();

	/// Tool activated
	void toolActivated() override;

	/// Tool disactivated
	void toolDisactivated() override;

	/// Point picked callback
	void pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccPointCloud* cloud, const CCVector3& P) override;
};

#endif
