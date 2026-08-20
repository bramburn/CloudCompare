// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_POINT_PICKING_GENERIC_INTERFACE_HEADER
#define CC_POINT_PICKING_GENERIC_INTERFACE_HEADER

/**
 * @file ccPointPickingGenericInterface.h
 *
 * @brief Generic point picking interface for interactive dialogs.
 *
 * @details Abstract base class for dialogs and tools that need to
 * pick points from 3D views. Provides a bridge between the Qt event
 * system and the picking infrastructure.
 *
 * Subclasses implement the processPickedPoint() method to define
 * what happens when a point is picked in the 3D view.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPickingHub
 * @see ccPickingListener
 * @see ccOverlayDialog
 */

// Local
#include "ccCommon.h"
#include "ccOverlayDialog.h"
#include "ccPickingListener.h"

// CCCoreLib
#include <CCGeom.h>

// system
#include <vector>

class ccGLWindowInterface;
class ccPointCloud;
class ccHObject;
class ccPickingHub;

/**
 * @brief Generic point picking interface for interactive dialogs.
 *
 * @details Abstract base class providing point picking functionality
 * for dialogs and overlay tools. Inherits from:
 * - ccOverlayDialog: for overlay dialog behavior
 * - ccPickingListener: for receiving picked point events
 *
 * Subclasses must implement processPickedPoint() to define
 * how picked points are handled.
 *
 * Usage pattern:
 * @code
 * class MyPickingTool : public ccPointPickingGenericInterface {
 * protected:
 *     void processPickedPoint(const PickedItem& picked) override {
 *         // Handle the picked point
 *         CCVector3 p = picked.point;
 *         // ...
 *     }
 * };
 * @endcode
 *
 * @extends ccOverlayDialog
 * @extends ccPickingListener
 */
class ccPointPickingGenericInterface : public ccOverlayDialog
    , public ccPickingListener
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the point picking interface.
	 *
	 * @param[in] pickingHub Picking hub for managing pick events.
	 * @param[in] parent Parent widget.
	 */
	explicit ccPointPickingGenericInterface(ccPickingHub* pickingHub, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccPointPickingGenericInterface() override = default;

	// inherited from ccOverlayDialog
	/**
	 * @brief Link with a 3D view for picking.
	 *
	 * @param[in] win Window to link with.
	 * @return true on success.
	 */
	bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the picking mode.
	 *
	 * @return true on success.
	 */
	bool start() override;

	/**
	 * @brief Stop the picking mode.
	 *
	 * @param[in] state Final state.
	 */
	void stop(bool state) override;

	// inherited from ccPickingListener
	/**
	 * @brief Handle a picked point.
	 *
	 * @param[in] pi Information about the picked point.
	 *
	 * @details Called when the user picks a point in the 3D view.
	 * Dispatches to the subclass's processPickedPoint().
	 */
	void onItemPicked(const PickedItem& pi) override;

  protected:
	/**
	 * @brief Process a picked point (subclass hook).
	 *
	 * @param[in] picked Information about the picked point.
	 *
	 * @details Subclasses implement this method to define
	 * what happens when a point is picked. The picked item
	 * contains:
	 * - 3D coordinates
	 * - Entity that was picked
	 * - Item index (point/triangle)
	 * - Screen coordinates
	 */
	virtual void processPickedPoint(const PickedItem& picked) = 0;

	//! Picking hub for managing pick events
	ccPickingHub* m_pickingHub;
};

#endif
