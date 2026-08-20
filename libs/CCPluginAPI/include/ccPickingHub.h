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
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

#include "CCPluginAPI.h"

/**
 * @file ccPickingHub.h
 *
 * @brief Picking hub for managing pick listeners
 *
 * Manages registration of picking listeners (tools that
 * respond to point/triangle clicks in 3D views).
 *
 * @author CloudCompare project
 */
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
 * @brief Picking hub
 *
 * Manages picking listeners for 3D view interactions.
 */
class CCPLUGIN_LIB_API ccPickingHub : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Create a picking hub
	 * @param[in] app Application interface
	 * @param[in] parent Parent object
	 */
	ccPickingHub(ccMainAppInterface* app, QObject* parent = nullptr);
	~ccPickingHub() override = default;

	/**
	 * @brief Get listener count
	 * @return Number of registered listeners
	 */
	inline size_t listenerCount() const
	{
		return m_listeners.size();
	}

	/**
	 * @brief Add a picking listener
	 * @param[in] listener Listener to add
	 * @param[in] exclusive Prevent new listeners
	 * @param[in] autoStartPicking Start picking mode on window
	 * @param[in] mode Picking mode
	 * @return true on success
	 */
	bool addListener(ccPickingListener*                listener,
	                 bool                              exclusive        = false,
	                 bool                              autoStartPicking = true,
	                 ccGLWindowInterface::PICKING_MODE mode             = ccGLWindowInterface::POINT_OR_TRIANGLE_PICKING);

	/**
	 * @brief Remove a picking listener
	 * @param[in] listener Listener to remove
	 * @param[in] autoStopPickingIfLast Stop if last listener
	 */
	void removeListener(ccPickingListener* listener, bool autoStopPickingIfLast = true);

	//	//! Sets the default picking mode
	//	/** \param mode picking mode
	//		\param autoEnableOnActivatedWindow whether picking mode should be enabled automatically on newly activated windows (if listeners are present only)
	//	**/
	// DGM: too dangerous, we can't change this behavior on the fly
	// void setPickingMode(ccGLWindowInterface::PICKING_MODE mode, bool autoEnableOnActivatedWindow = true);

	//! Manual start / stop of the picking mode on the active window
	void togglePickingMode(bool state);

	//! Returns the currently active window
	ccGLWindowInterface* activeWindow() const
	{
		return m_activeGLWindow;
	}

	//! Returns whether the picking mechanism is currently locked (i.e. an exclusive listener is registered)
	bool isLocked() const
	{
		return m_exclusive && !m_listeners.empty();
	}

  public:
	void onActiveWindowChanged(QMdiSubWindow*);
	void onActiveWindowDeleted(ccGLWindowInterface*);
	void processPickedItem(ccHObject*, unsigned, int, int, const CCVector3&, const CCVector3d&);

  protected:
	//! Listeners
	std::set<ccPickingListener*> m_listeners;

	//! Associated application
	ccMainAppInterface* m_app;

	//! Active GL window
	ccGLWindowInterface* m_activeGLWindow;

	//! Default picking mode
	ccGLWindowInterface::PICKING_MODE m_pickingMode;

	//! Automatically enables the picking mechanism on activated GL windows
	bool m_autoEnableOnActivatedWindow;

	//! Exclusive mode
	bool m_exclusive;
};
