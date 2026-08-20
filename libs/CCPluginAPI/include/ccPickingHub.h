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
// #                                                                        //
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPickingHub.h
 *
 * @brief Picking hub for managing 3D view pick listeners.
 *
 * @details Central manager for handling point/triangle picking in 3D views.
 *
 * ## Overview
 *
 * The picking hub routes picked items from 3D views to registered
 * listeners. It handles:
 * - Multiple listener registration
 * - Exclusive picking mode
 * - Automatic picking mode toggle
 * - Active window tracking
 *
 * ## Picking Modes
 *
 * - POINT_PICKING: Pick individual points
 * - TRIANGLE_PICKING: Pick mesh triangles
 * - POINT_OR_TRIANGLE_PICKING: Either
 * - EXPAND_CLOUD_PICKING: Expand cloud selection
 * - etc.
 *
 * ## Usage
 *
 * @code
 * // In tool initialization
 * if (!m_app->pickingHub()->addListener(this, true, true, mode)) {
 *     return false; // Picking already in use
 * }
 *
 * // In ccPickingListener implementation
 * void MyTool::onItemPicked(const PickedItem& pi) override
 * {
 *     // Handle picked point/triangle
 * }
 *
 * // Cleanup
 * m_app->pickingHub()->removeListener(this);
 * @endcode
 *
 * @author CloudCompare project
 *
 * @see ccPickingListener for listener interface
 * @see ccGLWindowInterface::PICKING_MODE for available modes
 */

#pragma once

#include "CCPluginAPI.h"

// Local
#include "ccPickingListener.h"

// qCC_gl
#include <ccGLWindowInterface.h>

// Qt
#include <QObject>

// system
#include <set>

class QMdiSubWindow;
class ccHObject;
class ccMainAppInterface;

/**
 * @brief Central manager for 3D picking operations.
 *
 * @details Routes picked items from GL windows to registered listeners.
 *
 * Features:
 * - Multiple listener support
 * - Exclusive picking mode
 * - Automatic picking toggle
 * - Window state tracking
 *
 * @extends QObject
 */
class CCPLUGIN_LIB_API ccPickingHub : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the picking hub.
	 *
	 * @param[in] app Application interface.
	 * @param[in] parent Parent object.
	 */
	ccPickingHub(ccMainAppInterface* app, QObject* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccPickingHub() override = default;

	/**
	 * @brief Get number of registered listeners.
	 *
	 * @return Listener count.
	 */
	size_t listenerCount() const
	{
		return m_listeners.size();
	}

	/**
	 * @brief Add a picking listener.
	 *
	 * @param[in] listener Listener to add.
	 * @param[in] exclusive Block other listeners.
	 * @param[in] autoStartPicking Auto-start picking mode.
	 * @param[in] mode Picking mode.
	 *
	 * @return true on success.
	 */
	bool addListener(ccPickingListener*                listener,
	                 bool                              exclusive        = false,
	                 bool                              autoStartPicking = true,
	                 ccGLWindowInterface::PICKING_MODE mode             = ccGLWindowInterface::POINT_OR_TRIANGLE_PICKING);

	/**
	 * @brief Remove a picking listener.
	 *
	 * @param[in] listener Listener to remove.
	 * @param[in] autoStopPickingIfLast Stop picking if last listener.
	 */
	void removeListener(ccPickingListener* listener, bool autoStopPickingIfLast = true);

	/**
	 * @brief Toggle picking mode on active window.
	 *
	 * @param[in] state Enable/disable.
	 */
	void togglePickingMode(bool state);

	/**
	 * @brief Get the active GL window.
	 *
	 * @return Active window, or nullptr.
	 */
	ccGLWindowInterface* activeWindow() const
	{
		return m_activeGLWindow;
	}

	/**
	 * @brief Check if picking is locked.
	 *
	 * @return true if exclusive listener active.
	 */
	bool isLocked() const
	{
		return m_exclusive && !m_listeners.empty();
	}

  public slots:
	/**
	 * @brief Handle active window change.
	 *
	 * @param[in] window New active window.
	 */
	void onActiveWindowChanged(QMdiSubWindow* window);

	/**
	 * @brief Handle window deletion.
	 *
	 * @param[in] window Deleted window.
	 */
	void onActiveWindowDeleted(ccGLWindowInterface* window);

	/**
	 * @brief Process a picked item.
	 *
	 * @param[in] object Picked entity.
	 * @param[in] subObjectIndex Sub-object index.
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 * @param[in] clickPoint Picked 3D point.
	 * @param[in] cloudPointsRelatedToEntity Cloud points.
	 */
	void processPickedItem(ccHObject*, unsigned, int, int, const CCVector3&, const CCVector3d&);

  private:
	//! Registered listeners
	std::set<ccPickingListener*> m_listeners;

	//! Application interface
	ccMainAppInterface* m_app;

	//! Active GL window
	ccGLWindowInterface* m_activeGLWindow;

	//! Default picking mode
	ccGLWindowInterface::PICKING_MODE m_pickingMode;

	//! Auto-enable on window activation
	bool m_autoEnableOnActivatedWindow;

	//! Exclusive mode
	bool m_exclusive;
};
