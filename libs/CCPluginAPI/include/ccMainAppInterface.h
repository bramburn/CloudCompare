// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccMainAppInterface.h
 *
 * @brief Main application interface for CloudCompare plugins.
 *
 * @details Defines the interface that plugins use to interact with
 * the CloudCompare application.
 *
 * ## Core Functions
 *
 * This interface provides access to:
 * - **Window Management**: Create/destroy 3D views, get active window
 * - **Entity Database**: Add/remove entities from the DB tree
 * - **Selection**: Get/set selected entities
 * - **Overlay Dialogs**: Register floating dialogs above 3D views
 * - **UI Control**: Freeze/unfreeze UI, update menus
 * - **Logging**: Display messages in the console
 *
 * ## Usage by Plugins
 *
 * Plugins receive a pointer to this interface via their start() method:
 *
 * @code
 * bool MyPlugin::start(ccMainAppInterface* app)
 * {
 *     // Add entity to DB
 *     app->addToDB(myPointCloud);
 *
 *     // Show message
 *     app->dispToConsole("Processing complete", ccMainAppInterface::STD_CONSOLE_MESSAGE);
 *
 *     // Get selected entities
 *     const auto& selected = app->getSelectedEntities();
 *
 *     return true;
 * }
 * @endcode
 *
 * ## Thread Safety
 *
 * Most methods should be called from the main Qt thread.
 * Avoid modifying entities or the DB tree from background threads.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPluginInterface for plugin interface
 * @see ccOverlayDialog for overlay dialog implementation
 */

#pragma once

// Qt
#include <QString>

// qCC_db
#include <ccHObject.h>
#include <ccHObjectCaster.h>

// qCC_gl
#include <ccGLUtils.h>

class QMainWindow;
class QWidget;
class ccGLWindowInterface;
class ccColorScalesManager;
class ccOverlayDialog;
class ccPickingHub;

/**
 * @brief Interface for plugin-application communication.
 *
 * @details Provides access to CloudCompare's core functionality
 * for plugins.
 *
 * ## Window Management
 *
 * - createGLWindow() / destroyGLWindow()
 * - getActiveGLWindow()
 * - redrawAll() / refreshAll()
 *
 * ## Entity Database
 *
 * - addToDB() / removeFromDB()
 * - dbRootObject()
 * - setSelectedInDB() / getSelectedEntities()
 *
 * ## UI Control
 *
 * - freezeUI() / updateUI()
 * - registerOverlayDialog() / unregisterOverlayDialog()
 * - disableAll() / enableAll()
 *
 * ## Logging
 *
 * - dispToConsole()
 * - forceConsoleDisplay()
 *
 * ## View Control
 *
 * - setView()
 * - toggleActiveWindow*()
 * - zoomOnSelectedEntities()
 */
class ccMainAppInterface
{
  public:
	/**
	 * @brief Destructor.
	 */
	virtual ~ccMainAppInterface() = default;

	/**
	 * @brief Get the main application window.
	 *
	 * @return Pointer to QMainWindow.
	 */
	virtual QMainWindow* getMainWindow() = 0;

	/**
	 * @brief Get the currently active 3D view.
	 *
	 * @return Pointer to active GL window, or nullptr.
	 */
	virtual ccGLWindowInterface* getActiveGLWindow() = 0;

	/**
	 * @brief Create a new 3D view window.
	 *
	 * @param[out] window Created GL window interface.
	 * @param[out] widget Encapsulating Qt widget.
	 *
	 * @warning Destroy via destroyGLWindow(), not directly.
	 *
	 * @see destroyGLWindow()
	 */
	virtual void createGLWindow(ccGLWindowInterface*& window, QWidget*& widget) const
	{
		window = nullptr;
		widget = nullptr;
	}

	/**
	 * @brief Destroy a GL window.
	 *
	 * @param[in] window Window to destroy.
	 */
	virtual void destroyGLWindow(ccGLWindowInterface*) const
	{
	}

	/**
	 * @brief Register an overlay dialog.
	 *
	 * Overlay dialogs float above 3D views and are automatically
	 * repositioned when the main window is resized.
	 *
	 * @param[in] dlg Dialog to register.
	 * @param[in] pos Position relative to MDI area corner.
	 *
	 * @see unregisterOverlayDialog()
	 */
	virtual void registerOverlayDialog(ccOverlayDialog* dlg, Qt::Corner pos)
	{
	}

	/**
	 * @brief Unregister an overlay dialog.
	 *
	 * @param[in] dlg Dialog to unregister.
	 */
	virtual void unregisterOverlayDialog(ccOverlayDialog* dlg)
	{
	}

	/**
	 * @brief Update overlay dialog positions.
	 */
	virtual void updateOverlayDialogsPlacement()
	{
	}

	/**
	 * @brief Get the unique ID generator.
	 *
	 * @return Unique ID generator.
	 */
	virtual ccUniqueIDGenerator::Shared getUniqueIDGenerator() = 0;

	/**
	 * @brief Load a file.
	 *
	 * @param[in] filename File to load.
	 * @param[in] silent Suppress error messages.
	 *
	 * @return Loaded entity, or nullptr.
	 */
	virtual ccHObject* loadFile(QString filename, bool silent) = 0;

	/**
	 * @brief Add entity to database.
	 *
	 * @param[in] obj Entity to add.
	 * @param[in] updateZoom Fit camera to new entity.
	 * @param[in] autoExpandDBTree Expand tree view.
	 * @param[in] checkDimensions Check dimensions.
	 * @param[in] autoRedraw Redraw views.
	 */
	virtual void addToDB(ccHObject* obj,
	                     bool       updateZoom       = false,
	                     bool       autoExpandDBTree = true,
	                     bool       checkDimensions  = false,
	                     bool       autoRedraw       = true) = 0;

	/**
	 * @brief Remove entity from database.
	 *
	 * @param[in] obj Entity to remove.
	 * @param[in] autoDelete Delete entity.
	 */
	virtual void removeFromDB(ccHObject* obj, bool autoDelete = true) = 0;

	/**
	 * @brief Context for temporary removal.
	 */
	struct ccHObjectContext
	{
		ccHObject* parent     = nullptr;
		int        childFlags = 0;
		int        parentFlags = 0;
	};

	/**
	 * @brief Remove object temporarily from DB tree.
	 *
	 * @param[in] obj Object to remove.
	 *
	 * @return Context for restoration.
	 *
	 * @see putObjectBackIntoDBTree()
	 */
	virtual ccHObjectContext removeObjectTemporarilyFromDBTree(ccHObject* obj)
	{
		return {};
	}

	/**
	 * @brief Restore object to DB tree.
	 *
	 * @param[in] obj Object to restore.
	 * @param[in] context Context from removal.
	 *
	 * @see removeObjectTemporarilyFromDBTree()
	 */
	virtual void putObjectBackIntoDBTree(ccHObject* obj, const ccHObjectContext& context)
	{
	}

	/**
	 * @brief Set entity selection state.
	 *
	 * @param[in] obj Entity to select/deselect.
	 * @param[in] selected Selection state.
	 */
	virtual void setSelectedInDB(ccHObject* obj, bool selected) = 0;

	/**
	 * @brief Get selected entities.
	 *
	 * @return Container of selected entities.
	 */
	virtual const ccHObject::Container& getSelectedEntities() const = 0;

	/**
	 * @brief Update properties panel.
	 */
	virtual void updatePropertiesView()
	{
	}

	/**
	 * @brief Check if any entity is selected.
	 *
	 * @return true if selection exists.
	 */
	bool haveSelection() const
	{
		return !getSelectedEntities().empty();
	}

	/**
	 * @brief Check if exactly one entity is selected.
	 *
	 * @return true if one entity selected.
	 */
	bool haveOneSelection() const
	{
		return getSelectedEntities().size() == 1;
	}

	/**
	 * @brief Console message levels.
	 */
	enum ConsoleMessageLevel
	{
		STD_CONSOLE_MESSAGE = 0, //!< Standard info message
		WRN_CONSOLE_MESSAGE = 1, //!< Warning message
		ERR_CONSOLE_MESSAGE = 2  //!< Error message
	};

	/**
	 * @brief Display message in console.
	 *
	 * @param[in] message Message text.
	 * @param[in] level Message level.
	 */
	virtual void dispToConsole(QString message, ConsoleMessageLevel level = STD_CONSOLE_MESSAGE) = 0;

	/**
	 * @brief Force console visibility.
	 */
	virtual void forceConsoleDisplay()
	{
	}

	/**
	 * @brief Get database root object.
	 *
	 * @return Root entity.
	 */
	virtual ccHObject* dbRootObject() = 0;

	/**
	 * @brief Redraw all 3D views.
	 *
	 * @param[in] only2D Only 2D elements.
	 */
	virtual void redrawAll(bool only2D = false) = 0;

	/**
	 * @brief Refresh views with pending updates.
	 *
	 * @param[in] only2D Only 2D elements.
	 */
	virtual void refreshAll(bool only2D = false) = 0;

	/**
	 * @brief Enable all GL windows.
	 */
	virtual void enableAll() = 0;

	/**
	 * @brief Disable all GL windows.
	 */
	virtual void disableAll() = 0;

	/**
	 * @brief Disable all except one window.
	 *
	 * @param[in] win Window to keep enabled.
	 */
	virtual void disableAllBut(ccGLWindowInterface* win) = 0;

	/**
	 * @brief Update UI to reflect selection state.
	 */
	virtual void updateUI() = 0;

	/**
	 * @brief Freeze/unfreeze UI.
	 *
	 * @param[in] state Freeze state.
	 */
	virtual void freezeUI(bool state) = 0;

	/**
	 * @brief Get color scales manager.
	 *
	 * @return Color scales manager.
	 */
	virtual ccColorScalesManager* getColorScalesManager()
	{
		return nullptr;
	}

	/**
	 * @brief Spawn histogram dialog.
	 *
	 * @param[in] histoValues Histogram bins.
	 * @param[in] minVal Minimum value.
	 * @param[in] maxVal Maximum value.
	 * @param[in] title Dialog title.
	 * @param[in] xAxisLabel X-axis label.
	 */
	virtual void spawnHistogramDialog(const std::vector<unsigned>& histoValues,
	                                  double                       minVal,
	                                  double                       maxVal,
	                                  QString                      title,
	                                  QString                      xAxisLabel)
	{
	}

	/**
	 * @brief Get picking hub.
	 *
	 * @return Picking hub, or nullptr.
	 */
	virtual ccPickingHub* pickingHub()
	{
		return nullptr;
	}

	/**
	 * @brief Set view orientation.
	 *
	 * @param[in] view View orientation.
	 */
	virtual void setView(CC_VIEW_ORIENTATION view) = 0;

	/**
	 * @brief Toggle centered perspective.
	 */
	virtual void toggleActiveWindowCenteredPerspective() = 0;

	/**
	 * @brief Toggle custom light.
	 */
	virtual void toggleActiveWindowCustomLight() = 0;

	/**
	 * @brief Toggle sun light.
	 */
	virtual void toggleActiveWindowSunLight() = 0;

	/**
	 * @brief Toggle viewer-based perspective.
	 */
	virtual void toggleActiveWindowViewerBasedPerspective() = 0;

	/**
	 * @brief Zoom on selected entities.
	 */
	virtual void zoomOnSelectedEntities() = 0;

	/**
	 * @brief Set global zoom.
	 */
	virtual void setGlobalZoom() = 0;

	/**
	 * @brief Increase point size.
	 */
	virtual void increasePointSize() = 0;

	/**
	 * @brief Decrease point size.
	 */
	virtual void decreasePointSize() = 0;
};
