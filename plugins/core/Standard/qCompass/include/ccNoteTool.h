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

#ifndef CC_NOTETOOL_HEADER
#define CC_NOTETOOL_HEADER

/**
 * @file ccNoteTool.h
 *
 * @brief Note tool
 *
 * Tool for creating notes and associating them with points.
 */

#include "ccTool.h"
#include "ccNote.h"

#include <QInputDialog>
#include <QMainWindow>

/**
 * @class ccNoteTool
 *
 * @brief Note tool
 *
 * Tool for creating notes and associating them with points in a cloud.
 */
class ccNoteTool : public ccTool
{
public:
	/// Constructor
	ccNoteTool();

	/// Tool activated
	void toolActivated() override;

	/// Tool disactivated
	void toolDisactivated() override;

	/// Point picked callback
	void pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccPointCloud* cloud, const CCVector3& P) override;
};

#endif
