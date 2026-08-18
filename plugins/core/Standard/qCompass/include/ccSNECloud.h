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

#ifndef CC_SNE_HEADER
#define CC_SNE_HEADER

/**
 * @file ccSNECloud.h
 *
 * @brief SNE cloud class
 *
 * Represents normal vectors for strike and dip measurements.
 */

#include <ccPointCloud.h>
#include <ccMeasurement.h>

/**
 * @class ccSNECloud
 *
 * @brief SNE cloud
 *
 * Represents/draws strike and dip normal vectors.
 */
class ccSNECloud : 
	public ccPointCloud,
	public ccMeasurement
{
public:
	/// Constructor
	ccSNECloud();
	
	/**
	 * @brief Create from point cloud
	 * @param[in] obj Source cloud
	 */
	ccSNECloud(ccPointCloud* obj);

	/// Update metadata
	void updateMetadata();

	/**
	 * @brief Check if object is an SNE cloud
	 * @param[in] obj Object to check
	 * @return True if SNE cloud
	 */
	static bool isSNECloud(ccHObject* obj);

protected:
	/// Draw object
	virtual void drawMeOnly(CC_DRAW_CONTEXT& context) override;
};
#endif
