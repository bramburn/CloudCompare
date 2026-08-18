//##########################################################################
//#                                                                        #
//#                              CLOUDCOMPARE                              #
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
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#ifndef PROFILE_LOADER_HEADER
#define PROFILE_LOADER_HEADER

/**
 * @file profileLoader.h
 *
 * @brief Profile loader
 *
 * Load 2D profiles from ASCII files.
 */

class ccPolyline;
class ccMainAppInterface;

//Qt
#include <QString>

//CCCoreLib
#include <CCGeom.h>

/**
 * @class ProfileLoader
 *
 * @brief Profile loader
 *
 * Load 2D profiles from custom ASCII files.
 */
class ProfileLoader
{
public:

	/**
	 * @brief Load profile from file
	 * @param[in] filename Filename
	 * @param[out] origin Profile origin
	 * @param[in] app Main application interface
	 * @return Loaded polyline or nullptr
	 *
	 * File must have a particular organization.
	 * X = radius, Y = height, Z = 0.
	 */
	static ccPolyline* Load(QString filename, CCVector3& origin, ccMainAppInterface* app = nullptr);

};

#endif //PROFILE_LOADER_HEADER
