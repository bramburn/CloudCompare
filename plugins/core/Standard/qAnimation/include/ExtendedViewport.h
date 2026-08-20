#pragma once

/**
 * @file ExtendedViewport.h
 *
 * @brief Extended viewport with custom light information.
 *
 * @details Viewport parameters for animation with custom light support.
 */

//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qAnimation                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#      COPYRIGHT: Daniel Girardeau-Montaut, CloudCompare project         #
//#                                                                        #
//##########################################################################

/**
 * @file ExtendedViewport.h
 *
 * @brief Extended viewport with custom light information
 *
 * Viewport parameters for animation with custom light support.
 */

//qCC_db
#include <cc2DViewportObject.h>

/**
 * @struct ExtendedViewportParameters
 *
 * @brief Extended viewport parameters
 *
 * Viewport parameters with custom light info.
 */
struct ExtendedViewportParameters
{
	ExtendedViewportParameters()
		: params{}
		, customLightEnabled(false)
		, customLightPos{}
	{}

	ExtendedViewportParameters(const ccViewportParameters& vpParams)
		: params(vpParams)
		, customLightEnabled(false)
		, customLightPos{}
	{}

	ccViewportParameters params;
	bool customLightEnabled;
	CCVector3f customLightPos;
};

/**
 * @struct ExtendedViewport
 *
 * @brief Extended viewport
 *
 * Viewport with custom light information for animation.
 */
struct ExtendedViewport
{
	//! Default constructor
	ExtendedViewport(cc2DViewportObject* vp = nullptr)
		: viewport(vp)
		, customLightEnabled(false)
		, customLightPos{}
	{
		if (viewport)
		{
			if (vp->hasMetaData("CustomLightPosX")
				&& vp->hasMetaData("CustomLightPosY")
				&& vp->hasMetaData("CustomLightPosZ"))
			{
				customLightPos.x = vp->getMetaData("CustomLightPosX").toFloat();
				customLightPos.y = vp->getMetaData("CustomLightPosY").toFloat();
				customLightPos.z = vp->getMetaData("CustomLightPosZ").toFloat();

				if (vp->hasMetaData("CustomLightEnabled"))
				{
					customLightEnabled = vp->getMetaData("CustomLightEnabled").toBool();
				}
			}
		}
	}

	ExtendedViewportParameters toExtendedViewportParameters() const
	{
		ExtendedViewportParameters evp;

		if (!viewport)
		{
			assert(false);
			return {};
		}

		evp.params = viewport->getParameters();
		evp.customLightEnabled = customLightEnabled;
		evp.customLightPos = customLightPos;

		return evp;
	}

	//! Viewport object.
	cc2DViewportObject* viewport;

	//! Custom light enabled flag.
	bool customLightEnabled;

	//! Custom light position.
	CCVector3f customLightPos;
};
