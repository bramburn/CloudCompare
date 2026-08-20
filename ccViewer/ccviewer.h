// ##########################################################################
// #                                                                        #
// #                   CLOUDCOMPARE LIGHT VIEWER                            #
// #                                                                        #
// #  This project has been initiated under funding from ANR/CIFRE          #
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
// #      +++ COPYRIGHT: EDF R&D + TELECOM ParisTech (ENST-TSI) +++         #
// #                                                                        //
// ##########################################################################

/**
 * @file ccviewer.h
 *
 * @brief ccViewer - lightweight point cloud viewer application.
 *
 * @details ccViewer is a simplified, read-only viewer application
 * for CloudCompare point cloud and mesh files.
 *
 * ## Overview
 *
 * Unlike the full CloudCompare application (qCC), ccViewer provides
 * a lightweight viewing experience with basic manipulation features:
 * - Single 3D view
 * - Basic entity display controls
 * - View orientation presets
 * - Camera manipulation
 *
 * ## Key Features
 *
 * - View point clouds and meshes
 * - Multiple file format support (via plugins)
 * - Standard and custom viewpoints
 * - Lighting control
 * - Scalar field visualization
 * - 3D mouse support
 *
 * ## Differences from qCC
 *
 * ccViewer is designed for:
 * - Quick file preview
 * - Simple visualization tasks
 * - Reduced memory footprint
 * - Faster startup time
 *
 * It lacks editing capabilities and advanced processing tools.
 *
 * @author EDF R&D + TELECOM ParisTech (ENST-TSI)
 *
 * @see ccMainAppInterface for interface requirements
 */

#ifndef CCVIEWER_HEADER
#define CCVIEWER_HEADER

// Qt
#include <QMainWindow>
#include <QStringList>

// CCPluginAPI
#include <ccMainAppInterface.h>

// GUIs
#include <ui_ccviewer.h>

// System
#include <set>

class ccGLWindowInterface;
class ccHObject;
class Mouse3DInput;

/**
 * @brief Lightweight viewer main window.
 *
 * @details Simplified viewer application for CloudCompare files.
 *
 * Features:
 * - Single 3D OpenGL window
 * - Basic entity manipulation
 * - View presets (front, back, iso, etc.)
 * - Lighting controls
 * - Scalar field display
 * - 3D mouse support
 *
 * @extends QMainWindow
 * @extends ccMainAppInterface
 */
class ccViewer : public QMainWindow
    , public ccMainAppInterface
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the viewer.
	 *
	 * @param[in] parent Parent widget.
	 * @param[in] flags Window flags.
	 */
	ccViewer(QWidget* parent = 0, Qt::WindowFlags flags = QFlags<Qt::WindowType>());

	/**
	 * @brief Destructor.
	 */
	~ccViewer() override;

	/**
	 * @brief Add entity to database.
	 *
	 * @param[in] entity Entity to add.
	 * @param[in] updateZoom Fit camera to entity.
	 * @param[in] autoExpandDBTree Expand tree view.
	 * @param[in] checkDimensions Check dimensions.
	 * @param[in] autoRedraw Redraw after adding.
	 */
	void addToDB(ccHObject* entity,
	             bool       updateZoom       = false,
	             bool       autoExpandDBTree = true,
	             bool       checkDimensions  = false,
	             bool       autoRedraw       = true) override;

	/**
	 * @brief Remove entity from database.
	 *
	 * @param[in] obj Entity to remove.
	 * @param[in] autoDelete Delete entity.
	 */
	void removeFromDB(ccHObject* obj, bool autoDelete = true) override;

	/**
	 * @brief Check if entities are loaded.
	 *
	 * @return true if entities exist in DB.
	 */
	bool checkForLoadedEntities();

  public:
	/**
	 * @brief Load files.
	 *
	 * @param[in] filenames Files to load.
	 *
	 * @return First loaded entity/group.
	 */
	ccHObject* addToDB(QStringList filenames);

  public: // ccMainInterface compliance
	/**
	 * @brief Get main window.
	 */
	QMainWindow* getMainWindow() override
	{
		return this;
	}

	/**
	 * @brief Get active GL window.
	 */
	ccGLWindowInterface* getActiveGLWindow() override
	{
		return m_glWindow;
	}

	/**
	 * @brief Load file.
	 *
	 * @param[in] filename File to load.
	 * @param[in] silent Suppress errors.
	 */
	ccHObject* loadFile(QString filename, bool silent) override
	{
		return addToDB(QStringList{filename});
	}

	/**
	 * @brief Set selection.
	 */
	void setSelectedInDB(ccHObject* obj, bool selected) override
	{
	}

	/**
	 * @brief Get selected entities.
	 */
	const ccHObject::Container& getSelectedEntities() const override;

	/**
	 * @brief Display console message.
	 *
	 * @param[in] message Message text.
	 * @param[in] level Message level.
	 */
	void dispToConsole(QString message, ConsoleMessageLevel level = STD_CONSOLE_MESSAGE) override;

	/**
	 * @brief Get database root.
	 */
	ccHObject* dbRootObject() override;

	/**
	 * @brief Redraw all views.
	 *
	 * @param[in] only2D Only 2D elements.
	 */
	void redrawAll(bool only2D = false) override;

	/**
	 * @brief Refresh all views.
	 *
	 * @param[in] only2D Only 2D elements.
	 */
	void refreshAll(bool only2D = false) override;

	/**
	 * @brief Enable all interactions.
	 */
	void enableAll() override;

	/**
	 * @brief Disable all interactions.
	 */
	void disableAll() override;

	/**
	 * @brief Disable all except one window.
	 *
	 * @param[in] win Window to keep active.
	 */
	void disableAllBut(ccGLWindowInterface* win) override;

	/**
	 * @brief Update UI.
	 */
	void updateUI() override
	{
	}

	/**
	 * @brief Freeze/unfreeze UI.
	 *
	 * @param[in] state Freeze state.
	 */
	void freezeUI(bool state) override
	{
	}

	/**
	 * @brief Set view orientation.
	 *
	 * @param[in] view View orientation.
	 */
	void setView(CC_VIEW_ORIENTATION view) override;

	/**
	 * @brief Toggle centered perspective.
	 */
	void toggleActiveWindowCenteredPerspective() override;

	/**
	 * @brief Toggle custom light.
	 */
	void toggleActiveWindowCustomLight() override;

	/**
	 * @brief Toggle sun light.
	 */
	void toggleActiveWindowSunLight() override;

	/**
	 * @brief Toggle viewer-based perspective.
	 */
	void toggleActiveWindowViewerBasedPerspective() override;

	/**
	 * @brief Zoom on selected entities.
	 */
	void zoomOnSelectedEntities() override
	{
		zoomOnSelectedEntity();
	}

	/**
	 * @brief Increase point size.
	 */
	void increasePointSize() override;

	/**
	 * @brief Decrease point size.
	 */
	void decreasePointSize() override;

	/**
	 * @brief Get unique ID generator.
	 */
	ccUniqueIDGenerator::Shared getUniqueIDGenerator() override;

  protected slots:
	/**
	 * @brief Show display parameters dialog.
	 */
	void showDisplayParameters();

	/**
	 * @brief Update display from parameters.
	 */
	void updateDisplay();

	/**
	 * @brief Select entity.
	 *
	 * @param[in] entity Entity to select.
	 */
	void selectEntity(ccHObject* entity);

	/**
	 * @brief Delete selected entity.
	 */
	void doActionDeleteSelectedEntity();

	/**
	 * @brief Handle fullscreen toggle.
	 *
	 * @param[in] state Fullscreen state.
	 */
	void onExclusiveFullScreenToggled(bool state);

	/**
	 * @brief Edit camera parameters.
	 */
	void doActionEditCamera();

	/**
	 * @brief Toggle sun light.
	 *
	 * @param[in] state Light state.
	 */
	void toggleSunLight(bool state);

	/**
	 * @brief Toggle custom light.
	 *
	 * @param[in] state Light state.
	 */
	void toggleCustomLight(bool state);

	/**
	 * @brief Toggle stereo mode.
	 *
	 * @param[in] state Stereo state.
	 */
	void toggleStereoMode(bool state);

	/**
	 * @brief Toggle fullscreen.
	 *
	 * @param[in] state Fullscreen state.
	 */
	void toggleFullScreen(bool state);

	/**
	 * @brief Toggle rotation about vertical axis.
	 */
	void toggleRotationAboutVertAxis();

	/**
	 * @brief Show about dialog.
	 */
	void doActionAbout();

	/**
	 * @brief Display shortcuts.
	 */
	void doActionDisplayShortcuts();

	/**
	 * @brief Set pivot always on.
	 */
	void setPivotAlwaysOn();

	/**
	 * @brief Set pivot rotation only.
	 */
	void setPivotRotationOnly();

	/**
	 * @brief Turn off pivot.
	 */
	void setPivotOff();

	/**
	 * @brief Set orthographic view.
	 */
	void setOrthoView();

	/**
	 * @brief Set centered perspective view.
	 */
	void setCenteredPerspectiveView();

	/**
	 * @brief Set viewer perspective view.
	 */
	void setViewerPerspectiveView();

	/**
	 * @brief Set global zoom.
	 */
	void setGlobalZoom() override;

	/**
	 * @brief Zoom on selected entity.
	 */
	void zoomOnSelectedEntity();

	// View presets
	/**
	 * @brief Set front view.
	 */
	void setFrontView();

	/**
	 * @brief Set bottom view.
	 */
	void setBottomView();

	/**
	 * @brief Set top view.
	 */
	void setTopView();

	/**
	 * @brief Set back view.
	 */
	void setBackView();

	/**
	 * @brief Set left view.
	 */
	void setLeftView();

	/**
	 * @brief Set right view.
	 */
	void setRightView();

	/**
	 * @brief Set isometric view 1.
	 */
	void setIsoView1();

	/**
	 * @brief Set isometric view 2.
	 */
	void setIsoView2();

	// Entity display controls
	/**
	 * @brief Toggle colors visibility.
	 *
	 * @param[in] state Visibility state.
	 */
	void toggleColorsShown(bool state);

	/**
	 * @brief Toggle normals visibility.
	 *
	 * @param[in] state Visibility state.
	 */
	void toggleNormalsShown(bool state);

	/**
	 * @brief Toggle materials visibility.
	 *
	 * @param[in] state Visibility state.
	 */
	void toggleMaterialsShown(bool state);

	/**
	 * @brief Toggle scalar field visibility.
	 *
	 * @param[in] state Visibility state.
	 */
	void toggleScalarShown(bool state);

	/**
	 * @brief Toggle colorbar visibility.
	 *
	 * @param[in] state Visibility state.
	 */
	void toggleColorbarShown(bool state);

	/**
	 * @brief Change current scalar field.
	 *
	 * @param[in] state Change state.
	 */
	void changeCurrentScalarField(bool state);

	// 3D mouse
	/**
	 * @brief Handle 3D mouse move.
	 *
	 * @param[in] data Mouse data.
	 */
	void on3DMouseMove(std::vector<float>& data);

	/**
	 * @brief Handle 3D mouse key up.
	 *
	 * @param[in] key Key code.
	 */
	void on3DMouseKeyUp(int key);

	/**
	 * @brief Handle 3D mouse key down.
	 *
	 * @param[in] key Key code.
	 */
	void on3DMouseKeyDown(int key);

	/**
	 * @brief Handle 3D mouse CMD key down.
	 *
	 * @param[in] key Key code.
	 */
	void on3DMouseCMDKeyDown(int key);

	/**
	 * @brief Handle 3D mouse CMD key up.
	 *
	 * @param[in] key Key code.
	 */
	void on3DMouseCMDKeyUp(int key);

	/**
	 * @brief Handle 3D mouse released.
	 */
	void on3DMouseReleased();

	/**
	 * @brief Enable/disable 3D mouse.
	 *
	 * @param[in] state Enable state.
	 */
	void enable3DMouse(bool state);

	// GL filters
	/**
	 * @brief Enable GL filter.
	 */
	void doEnableGLFilter();

	/**
	 * @brief Disable GL filter.
	 */
	void doDisableGLFilter();

	/**
	 * @brief Select next scalar field.
	 *
	 * @param[in] deltaPos Direction.
	 */
	void selectNextSF(int deltaPos);

  protected:
	/**
	 * @brief Load plugins.
	 */
	void loadPlugins();

	/**
	 * @brief Update GL frame gradient.
	 */
	void updateGLFrameGradient();

	/**
	 * @brief Reflect perspective state.
	 */
	void reflectPerspectiveState();

	/**
	 * @brief Reflect pivot state.
	 */
	void reflectPivotVisibilityState();

	/**
	 * @brief Reflect lights state.
	 */
	void reflectLightsState();

	/**
	 * @brief Check stereo mode.
	 *
	 * @return true if stereo can be stopped.
	 */
	bool checkStereoMode();

	/**
	 * @brief Release 3D mouse.
	 */
	void release3DMouse();

  private:
	//! GL window
	ccGLWindowInterface* m_glWindow;

	//! Selected object
	ccHObject* m_selectedObject;

	//! 3D mouse handler
	Mouse3DInput* m_3dMouseInput;

	//! UI definition
	Ui::ccViewerClass ui;
};

#endif // CCVIEWER_HEADER
