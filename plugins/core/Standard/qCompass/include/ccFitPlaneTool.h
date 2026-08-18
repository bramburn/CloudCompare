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

#ifndef CC_FITPLANETOOL_HEADER
#define CC_FITPLANETOOL_HEADER

/**
 * @file ccFitPlaneTool.h
 *
 * @brief Fit plane tool
 *
 * Tool for creating fit planes from point picks.
 */

#include <DgmOctreeReferenceCloud.h>

#include "ccTool.h"
#include "ccMouseCircle.h"
#include "ccFitPlane.h"

/**
 * @class ccFitPlaneTool
 *
 * @brief Fit plane tool
 *
 * Tool for creating fit planes during "Plane Mode".
 */
class ccFitPlaneTool : public ccTool
{
public:
	/// Constructor
	ccFitPlaneTool();
	
	/// Destructor
	~ccFitPlaneTool() override;

	/// Tool activated
	void toolActivated() override;

	/// Tool disactivated
	void toolDisactivated() override;

	/// Point picked callback
	void pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccPointCloud* cloud, const CCVector3& P) override;

	/// Mouse circle for selection
	ccMouseCircle* m_mouseCircle = nullptr;
};

#endif
