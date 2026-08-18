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

#ifndef CC_TRACETOOL_HEADER
#define CC_TRACETOOL_HEADER

/**
 * @file ccTraceTool.h
 *
 * @brief Trace tool
 *
 * Tool for digitizing traces.
 */

#include "ccTool.h"
#include "ccTrace.h"

/**
 * @class ccTraceTool
 *
 * @brief Trace tool
 *
 * Tool for digitizing fracture traces.
 */
class ccTraceTool :	public ccTool
{
public:
	/// Constructor
	ccTraceTool();

	/// Tool activated
	void toolActivated() override;

	/// Tool disactivated
	void toolDisactivated() override;

	/// Point picked callback
	void pointPicked(ccHObject* insertPoint, unsigned itemIdx, ccPointCloud* cloud, const CCVector3& P) override;

	/// Selection changed callback
	void onNewSelection(const ccHObject::Container& selectedEntities) override;

	/// Accept action
	void accept() override;

	/// Cancel action
	void cancel() override;

	/// Check if undo is available
	bool canUndo() override;

	/// Undo last action
	void undo() override;

protected:
	/// Finish current trace
	void finishCurrentTrace();
	
	/// Pick up existing trace
	bool pickupTrace(ccHObject* obj);
	
	/// Active trace id
	int m_trace_id = -1;
	
	/// True if trace was picked up (not created)
	bool m_preExisting = false;
	
	/// True if changes were made
	bool m_changed = false;

	/// True if parent plane was deleted
	bool m_parentPlaneDeleted = false;
	
	/// True if child plane was deleted
	bool m_childPlaneDeleted = false;

	/// Precompute gradient for cost function
	bool m_precompute_gradient = true;
	
	/// Precompute curvature for cost functions
	bool m_precompute_curvature = true;
};

#endif
