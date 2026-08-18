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

#ifndef CC_THICKNESSTOOL_HEADER
#define CC_THICKNESSTOOL_HEADER

/**
 * @file ccThicknessTool.h
 *
 * @brief Thickness tool
 *
 * Tool for creating thickness measurements.
 */

#include "ccTool.h"
#include "ccThickness.h"
#include "ccPlane.h"

#include <ccColorTypes.h>
#include <DistanceComputationTools.h>

/**
 * @class ccThicknessTool
 *
 * @brief Thickness tool
 *
 * Tool for creating thickness measurements in qCompass.
 */
class ccThicknessTool :	public ccTool
{
public:
	/// Constructor
	ccThicknessTool();

	/// Tool activated
	void toolActivated() override;

	/// Tool disactivated
	void toolDisactivated() override;

	/// Selection changed callback
	void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/// Point picked callback (HObject version)
	bool pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccHObject* pickedObject, const CCVector3& P) override;

	/// Point picked callback
	void pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccPointCloud* cloud, const CCVector3& P) override;

	/// Accept action
	void accept() override;

	/// Cancel action
	void cancel() override;

protected:
	/// Reference plane for thickness calculation
	ccPlane* m_referencePlane = nullptr;
	
	/// Start point for two-point mode
	CCVector3* m_startPoint = nullptr;
	
	/// Hidden objects during measurement
	std::vector<int> m_hiddenObjects;
	
	/// Partially completed graphic
	int m_graphic_id = -1;

private:
	/// Calculate point-to-plane distance
	float planeToPointDistance(ccPlane* plane, CCVector3 P);

	/// Build thickness graphic
	ccHObject* buildGraphic(CCVector3 endPoint, float thickness);

	/// Recurse children for visibility
	void recurseChildren(ccHObject* obj, bool hidePointClouds, bool hidePlanes);

	/// Get interior of GeoObject
	ccHObject* getInsertInterior(ccHObject* insertPoint);

public:
	/// Two-point mode flag
	static bool TWO_POINT_MODE;
};

#endif
