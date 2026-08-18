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

#ifndef CC_LINEATIONTOOL_HEADER
#define CC_LINEATIONTOOL_HEADER

/**
 * @file ccLineationTool.h
 *
 * @brief Lineation tool
 *
 * Tool for creating and measuring lineations.
 */

#include "ccTool.h"
#include "ccLineation.h"

/**
 * @class ccLineationTool
 *
 * @brief Lineation tool
 *
 * Tool for creating/measuring lineations.
 */
class ccLineationTool :	public ccTool
{
public:
	/// Constructor
	ccLineationTool();

	/// Tool disactivated
	void toolDisactivated() override;

	/// Point picked callback
	void pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccPointCloud* cloud, const CCVector3& P) override;

	/// Accept action
	void accept() override;

	/// Cancel action
	void cancel() override;

protected:
	/// ID of lineation being written
	int m_lineation_id = -1;
};

#endif
