/**
 * @file cc3DMouseManager.h
 *
 * @brief 3DConnexion space mouse input manager
 *
 * Manages 3DConnexion (formerly Logitech) space mouse input devices
 * for CloudCompare navigation. The space mouse provides 6-DOF
 * (six degrees of freedom) input: translation (X/Y/Z) and rotation
 * (pitch/yaw/roll) simultaneously.
 *
 * The manager:
 * - Detects and manages a connected 3D mouse via hidapi
 * - Translates raw HID events into camera manipulation commands
 * - Exposes a menu for enabling/disabling the device
 * - Forwards motion events to the active ccGLWindow
 *
 * Supported 3D mice: SpaceMouse, SpaceMouse Pro, SpaceNavigator, etc.
 * Uses the hidapi library (vendored under libs/CCAppCommon/devices/3dConnexion/extern/).
 *
 * @extends QObject
 */

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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

#include "CCAppCommon.h"

#include <QObject>

class QAction;
class QMenu;

class ccMainAppInterface;
class Mouse3DInput;

/**
 * @class cc3DMouseManager
 *
 * @brief 3D mouse (space mouse) input manager
 *
 * Manages the connection to a 3DConnexion space mouse and forwards
 * 6-DOF input events to the active GL window for camera manipulation.
 *
 * Key responsibilities:
 * - Device initialization via hidapi
 * - Translation and rotation input forwarding
 * - Menu with enable/disable toggle
 * - Proper cleanup on destruction
 *
 * @extends QObject
 */
class CCAPPCOMMON_LIB_API cc3DMouseManager : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the manager
	 *
	 * @param[in] appInterface Main application interface
	 * @param[in] parent Parent QObject
	 */
	cc3DMouseManager(ccMainAppInterface* appInterface, QObject* parent);

	//! Destructor — releases the device and cleans up
	~cc3DMouseManager();

	/**
	 * @brief Get the 3D mouse menu
	 *
	 * @return The QMenu for 3D mouse settings (enable/disable, etc.)
	 */
	QMenu* menu()
	{
		return m_menu;
	}

  private:
	/**
	 * @brief Enable or disable the 3D mouse device
	 *
	 * @param[in] state true to enable, false to disable
	 * @param[in] silent Suppress error messages if true
	 */
	void enableDevice(bool state, bool silent);

	/**
	 * @brief Release (disconnect) the 3D mouse device
	 */
	void releaseDevice();

	//! Set up the 3D mouse menu and actions
	void setupMenu();

	// HID event handlers

	/**
	 * @brief Handle 3D mouse button release (raw HID)
	 *
	 * @param[in] key Button identifier
	 */
	void on3DMouseKeyUp(int key);

	/**
	 * @brief Handle 3D mouse command button release (raw HID)
	 *
	 * @param[in] cmd Command button identifier
	 */
	void on3DMouseCMDKeyUp(int cmd);

	/**
	 * @brief Handle 3D mouse button press (raw HID)
	 *
	 * @param[in] key Button identifier
	 */
	void on3DMouseKeyDown(int key);

	/**
	 * @brief Handle 3D mouse command button press (raw HID)
	 *
	 * @param[in] cmd Command button identifier
	 */
	void on3DMouseCMDKeyDown(int cmd);

	/**
	 * @brief Handle 3D mouse motion (6-DOF movement)
	 *
	 * Called continuously while the device is moved.
	 * Forwards translation and rotation to the active GL window.
	 *
	 * @param[in] vec Float vector: [tx, ty, tz, rx, ry, rz]
	 *                Translation in mm, rotation in degrees
	 */
	void on3DMouseMove(std::vector<float>& vec);

	/**
	 * @brief Handle 3D mouse button release (mouse released)
	 *
	 * Signals the end of a motion event sequence.
	 */
	void on3DMouseReleased();

	//! Main application interface (for accessing the active window)
	ccMainAppInterface* m_appInterface;

	//! HID device wrapper (hidapi)
	Mouse3DInput* m3dMouseInput;

	//! Settings menu shown in the UI
	QMenu* m_menu;
	//! Enable/disable toggle action
	QAction* m_actionEnable;
};
