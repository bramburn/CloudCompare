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

#ifndef CC_NOTE_HEADER
#define CC_NOTE_HEADER

/**
 * @file ccNote.h
 *
 * @brief Note class
 *
 * Simple class for representing notes.
 */

#include "ccPointPair.h"

#include <ccPointCloud.h>

/**
 * @class ccNote
 *
 * @brief Note
 *
 * Represents notes created with qCompass.
 */
class ccNote : public ccPointPair
{
public:
	/**
	 * @brief Create note
	 * @param[in] associatedCloud Associated point cloud
	 */
	ccNote(ccPointCloud* associatedCloud);
	
	/**
	 * @brief Create from polyline
	 * @param[in] obj Source polyline
	 */
	ccNote(ccPolyline* obj);

	/// Update metadata
	void updateMetadata() override;

	/**
	 * @brief Check if object is a note
	 * @param[in] obj Object to check
	 * @return True if note
	 */
	static bool isNote(ccHObject* obj);
};

#endif
