// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPickingListener.h
 *
 * @brief Picking listener interface for 3D view interactions.
 *
 * @details Interface for tools that respond to point/triangle clicks
 * in 3D views.
 *
 * ## Overview
 *
 * Picking listeners receive notifications when users click on
 * entities in 3D views. This is used by:
 * - Interactive transformation tools
 * - Point picking dialogs
 * - Measurement tools
 * - Annotation tools
 *
 * ## Usage
 *
 * @code
 * class MyTool : public ccOverlayDialog, public ccPickingListener
 * {
 * public:
 *     void onItemPicked(const PickedItem& pi) override
 *     {
 *         if (pi.entity) {
 *             // Process picked point/triangle
 *             auto clickPos = pi.clickPoint;
 *             auto pickedPoint = pi.P3D;
 *             auto entity = pi.entity;
 *         }
 *     }
 * };
 * @endcode
 *
 * ## PickedItem Details
 *
 * - **clickPoint**: Screen coordinates of click
 * - **entity**: The entity that was clicked
 * - **itemIndex**: Point or triangle index
 * - **P3D**: 3D world coordinates of pick
 * - **uvw**: Barycentric coordinates (for triangles)
 * - **entityCenter**: True if entity center was picked
 *
 * @author CloudCompare project
 *
 * @see ccPickingHub for managing listeners
 * @see ccGLWindowInterface for picking modes
 */

#pragma once

#include "CCPluginAPI.h"

// CCCoreLib
#include <CCGeom.h>

// Qt
#include <QPoint>

class ccHObject;

/**
 * @brief Interface for handling 3D view pick events.
 *
 * @details Implement this interface to receive notifications
 * when users click on entities in 3D views.
 *
 * Common use cases:
 * - Interactive tools (transformation, annotation)
 * - Point/triangle picking
 * - Measurement tools
 * - Selection tools
 */
class CCPLUGIN_LIB_API ccPickingListener
{
  public:
	/**
	 * @brief Destructor.
	 */
	virtual ~ccPickingListener() = default;

	/**
	 * @brief Information about a picked item.
	 *
	 * Contains all relevant data about a user click
	 * on a 3D entity.
	 */
	struct PickedItem
	{
		/**
		 * @brief Default constructor.
		 */
		PickedItem()
		    : entity(nullptr)
		    , itemIndex(0)
		    , entityCenter(false)
		{
		}

		//! Screen click position
		QPoint clickPoint;

		//! Picked entity
		ccHObject* entity;

		//! Point or triangle index
		unsigned itemIndex;

		//! 3D picked point coordinates
		CCVector3 P3D;

		//! Barycentric coordinates (for triangles)
		CCVector3d uvw;

		//! True if entity center was picked
		bool entityCenter;
	};

	/**
	 * @brief Handle a picked item.
	 *
	 * @param[in] pi Information about the pick.
	 *
	 * Called when the user clicks on an entity
	 * while picking mode is active.
	 */
	virtual void onItemPicked(const PickedItem& pi) = 0;
};
