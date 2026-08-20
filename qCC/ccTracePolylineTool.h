#pragma once

// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccTracePolylineTool.h
 *
 * @brief Interactive polyline tracing tool.
 *
 * @details Tool for drawing polylines directly on the 3D view.
 *
 * Features:
 * - Click to add vertices
 * - Right-click to close polyline
 * - Automatic projection to 3D
 * - Width/thickness configuration
 * - Export to database
 *
 * Usage:
 * 1. Start the tool
 * 2. Click in 3D view to add vertices
 * 3. Right-click to close
 * 4. Apply to create the polyline
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccOverlayDialog
 * @see ccPickingListener
 */

#include "ccOverlayDialog.h"
#include "ccPickingListener.h"

#include <ccGenericGLDisplay.h>

// system
#include <vector>

class ccPolyline;
class ccPointCloud;
class ccGLWindowInterface;
class ccPickingHub;

namespace Ui
{
	class TracePolyLineDlg;
}

/**
 * @brief Interactive polyline tracing tool.
 *
 * @details Overlay dialog for drawing polylines in the 3D view.
 *
 * Workflow:
 * 1. Click to add vertices
 * 2. Preview shows current segment
 * 3. Right-click or close button to complete
 * 4. Apply to create the 3D polyline
 *
 * @extends ccOverlayDialog
 * @extends ccPickingListener
 */
class ccTracePolylineTool : public ccOverlayDialog
    , public ccPickingListener
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the trace polyline tool.
	 *
	 * @param[in] pickingHub Picking hub for 3D picking.
	 * @param[in] parent Parent widget.
	 */
	explicit ccTracePolylineTool(ccPickingHub* pickingHub, QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccTracePolylineTool();

	// inherited from ccOverlayDialog
	/**
	 * @brief Link with a 3D window.
	 */
	virtual bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the tool.
	 */
	virtual bool start() override;

	/**
	 * @brief Stop the tool.
	 *
	 * @param[in] accepted Whether to apply changes.
	 */
	virtual void stop(bool accepted) override;

  protected slots:
	/**
	 * @brief Apply the polyline.
	 */
	void apply();

	/**
	 * @brief Cancel and close.
	 */
	void cancel();

	/**
	 * @brief Export the polyline.
	 */
	void exportLine();

	/**
	 * @brief Continue edition mode.
	 */
	inline void continueEdition()
	{
		restart(false);
	}

	/**
	 * @brief Reset and start new polyline.
	 */
	inline void resetLine()
	{
		restart(true);
	}

	/**
	 * @brief Close the polyline.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 */
	void closePolyLine(int x = 0, int y = 0);

	/**
	 * @brief Update polyline preview.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 * @param[in] buttons Mouse buttons.
	 */
	void updatePolyLineTip(int x, int y, Qt::MouseButtons buttons);

	/**
	 * @brief Handle width change.
	 *
	 * @param[in] index New width.
	 */
	void onWidthSizeChanged(int index);

	/**
	 * @brief Handle shortcut trigger.
	 *
	 * @param[in] id Shortcut ID.
	 */
	void onShortcutTriggered(int id);

	/**
	 * @brief Handle item picked.
	 *
	 * @param[in] pi Picked item.
	 */
	virtual void onItemPicked(const PickedItem& pi) override;

  protected:
	/**
	 * @brief Restart the tool.
	 *
	 * @param[in] reset If true, clear all points.
	 */
	void restart(bool reset);

	/**
	 * @brief Viewport parameters for segment picking.
	 */
	struct SegmentGLParams
	{
		SegmentGLParams()
		{
		}

		SegmentGLParams(ccGenericGLDisplay* display, int x, int y);

		//! Camera parameters
		ccGLCameraParameters params;

		//! Click position
		CCVector2d clickPos;
	};

	/**
	 * @brief Oversample the polyline.
	 *
	 * @param[in] steps Number of interpolation steps.
	 * @return Oversampled polyline.
	 */
	ccPolyline* polylineOverSampling(unsigned steps) const;

  private:
	//! 2D preview polyline
	ccPolyline* m_polyTip;

	//! Preview vertices
	ccPointCloud* m_polyTipVertices;

	//! 3D polyline
	ccPolyline* m_poly3D;

	//! 3D vertices
	ccPointCloud* m_poly3DVertices;

	//! Viewport params per segment
	std::vector<SegmentGLParams> m_segmentParams;

	//! Process state
	bool m_done;

	//! Picking hub
	ccPickingHub* m_pickingHub;

	//! UI definition
	Ui::TracePolyLineDlg* m_ui;
};
