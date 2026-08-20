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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

// Qt
#include <QMainWindow>

// Local
#include "ccEntityAction.h"
#include "ccMainAppInterface.h"
#include "ccPickingListener.h"

// CCCoreLib
#include <AutoSegmentationTools.h>

class QAction;
class QMdiArea;
class QMdiSubWindow;
class QToolBar;
class QToolButton;

class cc3DMouseManager;
class ccCameraParamEditDlg;
class ccClippingBoxTool;
class ccComparisonDlg;
class ccDBRoot;
class ccDrawableObject;
class ccGLWindowInterface;
class ccGraphicalSegmentationTool;
class ccGraphicalTransformationTool;
class ccHObject;
class ccOverlayDialog;
class ccPluginUIManager;
class ccPointListPickingDlg;
class ccPointPairRegistrationDlg;
class ccPointPropertiesDlg;
class ccPrimitiveFactoryDlg;
class ccRecentFiles;
class ccSectionExtractionTool;
class ccStdPluginInterface;
class ccTracePolylineTool;
class ccShortcutDialog;

struct dbTreeSelectionInfo;

namespace Ui
{
	class MainWindow;
}

/**
 * @file mainwindow.h
 *
 * @brief CloudCompare Main Window - central application window.
 *
 * @details MainWindow is the central UI component of CloudCompare,
 * implementing the MDI (Multiple Document Interface) paradigm for
 * managing multiple 3D views.
 *
 * ## Architecture
 *
 * MainWindow serves three main roles:
 * 1. **Main application window** - menus, toolbars, dock widgets
 * 2. **ccMainAppInterface implementation** - plugin communication
 * 3. **ccPickingListener** - 3D point picking operations
 *
 * ## Key Components
 *
 * ### Window Management
 * - MDI sub-windows for multiple 3D views
 * - Synchronized views support
 * - Stereo vision mode
 *
 * ### Database Tree (DB Root)
 * - Hierarchical entity display
 * - Point clouds, meshes, primitives, sensors
 * - Drag-and-drop reordering
 * - Entity properties panel
 *
 * ### Tools & Actions
 * - Segmentation tool
 * - Transformation tool
 * - Section extraction
 * - Primitive creation
 *
 * ### Plugin System
 * - Standard plugins (processing algorithms)
 * - I/O plugins (file formats)
 * - GL plugins (OpenGL effects)
 *
 * ## Singleton Pattern
 *
 * MainWindow uses a singleton pattern. Access via TheInstance().
 *
 * ## Usage
 *
 * @code
 * MainWindow* mw = MainWindow::TheInstance();
 * mw->addToDB(entity);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccMainAppInterface for plugin interface
 * @see ccGLWindowInterface for 3D rendering
 * @see ccDBRoot for database tree
 */
class MainWindow : public QMainWindow
    , public ccMainAppInterface
    , public ccPickingListener
{
	Q_OBJECT

  protected:
	//! Default constructor
	MainWindow();

	//! Default desctructor
	~MainWindow() override;

  public:
	/**
	 * @brief Get the singleton instance of MainWindow
	 *
	 * MainWindow follows a singleton pattern with one global instance.
	 * Use this method to access the main window from anywhere in the application.
	 *
	 * @return Pointer to the MainWindow instance, or nullptr if not yet created
	 *
	 * @note In test environments or headless mode, this may return nullptr.
	 * @see DestroyInstance() to clean up
	 */
	static MainWindow* TheInstance();

	/**
	 * @brief Get the currently active 3D view
	 *
	 * Convenience static method that delegates to the singleton instance.
	 *
	 * @return Pointer to the active ccGLWindowInterface, or nullptr if no window is active
	 * @see getActiveGLWindow()
	 */
	static ccGLWindowInterface* GetActiveGLWindow();

	/**
	 * @brief Find a 3D view by its window title
	 *
	 * Searches all MDI sub-windows for one with the specified title.
	 *
	 * @param[in] title The window title to search for
	 * @return Pointer to the matching ccGLWindowInterface, or nullptr if not found
	 */
	static ccGLWindowInterface* GetGLWindow(const QString& title);

	/**
	 * @brief Get all open 3D views
	 *
	 * Populates a vector with pointers to all open 3D view windows.
	 *
	 * @param[in,out] glWindows Vector to populate with GL window pointers
	 */
	static void GetGLWindows(std::vector<ccGLWindowInterface*>& glWindows);

	/**
	 * @brief Refresh all 3D views
	 *
	 * Forces a repaint of all open 3D windows.
	 *
	 * @param[in] only2D If true, only refresh 2D displays (faster)
	 * @see refreshAll()
	 */
	static void RefreshAllGLWindow(bool only2D = false);

	/**
	 * @brief Update all UI elements
	 *
	 * Refreshes menus, toolbars, and other UI components to reflect
	 * the current state (e.g., selection changes).
	 *
	 * @see updateUI()
	 */
	static void UpdateUI();

	/**
	 * @brief Destroy the singleton instance
	 *
	 * Called during application shutdown to clean up the MainWindow.
	 * All MDI sub-windows should be closed before calling this.
	 *
	 * @see TheInstance()
	 */
	static void DestroyInstance();

	/**
	 * @brief Get the currently active 3D view
	 * @return Pointer to the active ccGLWindowInterface, or nullptr if none is active
	 */
	ccGLWindowInterface* getActiveGLWindow() override;

	/**
	 * @brief Get the MDI wrapper for a 3D view
	 * @param[in] win The 3D view window
	 * @return Pointer to the QMdiSubWindow wrapping this view, or nullptr
	 */
	QMdiSubWindow* getMDISubWindow(ccGLWindowInterface* win);

	/**
	 * @brief Get a 3D view by index
	 * @param[in] index Zero-based index of the view to retrieve
	 * @return Pointer to the ccGLWindowInterface, or nullptr if index out of range
	 */
	ccGLWindowInterface* getGLWindow(int index) const;

	/**
	 * @brief Get the number of open 3D views
	 * @return Count of currently open MDI sub-windows
	 */
	int getGLWindowCount() const;

	/**
	 * @brief Load files into the database
	 * @param[in] filenames List of file paths to load
	 * @param[in] fileFilter Optional file type filter
	 * @param[in] destWin Optional destination 3D window for immediate display
	 * @see loadFile() for loading a single file
	 */
	virtual void addToDB(const QStringList&   filenames,
	                     QString              fileFilter = QString(),
	                     ccGLWindowInterface* destWin    = nullptr);

	/**
	 * @brief Add an entity to the database
	 * @param[in] obj The entity to add
	 * @param[in] updateZoom Adjust camera to fit the new entity
	 * @param[in] autoExpandDBTree Expand tree to show the new entity
	 * @param[in] checkDimensions Warn if entity dimensions are unusual
	 * @param[in] autoRedraw Refresh 3D views after adding
	 */
	void addToDB(ccHObject* obj,
	             bool       updateZoom       = false,
	             bool       autoExpandDBTree = true,
	             bool       checkDimensions  = false,
	             bool       autoRedraw       = true) override;

	/**
	 * @brief Register an overlay dialog
	 * @param[in] dlg The overlay dialog to register
	 * @param[in] pos The screen corner to anchor the dialog
	 */
	void registerOverlayDialog(ccOverlayDialog* dlg, Qt::Corner pos) override;
	/**
	 * @brief Remove an overlay dialog
	 * @param[in] dlg The overlay dialog to unregister
	 */
	void unregisterOverlayDialog(ccOverlayDialog* dlg) override;
	/**
	 * @brief Reposition all overlay dialogs (called on resize)
	 */
	void updateOverlayDialogsPlacement() override;
	/**
	 * @brief Remove an entity from the database
	 * @param[in] obj The entity to remove
	 * @param[in] autoDelete Delete the entity immediately if true
	 */
	void removeFromDB(ccHObject* obj, bool autoDelete = true) override;
	/**
	 * @brief Set the selection state of an entity
	 * @param[in] obj The entity to select/deselect
	 * @param[in] selected True to select, false to deselect
	 */
	void setSelectedInDB(ccHObject* obj, bool selected) override;
	/**
	 * @brief Display a message in the console
	 * @param[in] message The message to display
	 * @param[in] level Message severity (INFO, WARNING, ERROR)
	 */
	void dispToConsole(QString message, ConsoleMessageLevel level = STD_CONSOLE_MESSAGE) override;
	/**
	 * @brief Force the console to become visible
	 */
	void forceConsoleDisplay() override;
	/**
	 * @brief Get the database root object
	 * @return Pointer to the root ccHObject
	 */
	ccHObject* dbRootObject() override;
	inline QMainWindow* getMainWindow() override
	{
		return this;
	}
	/**
	 * @brief Load a single file into the database
	 * @param[in] filename Path to the file to load
	 * @param[in] silent Suppress error dialogs if true
	 * @return Pointer to the loaded entity, or nullptr on failure
	 */
	ccHObject* loadFile(QString filename, bool silent) override;
	inline const ccHObject::Container& getSelectedEntities() const override
	{
		return m_selectedEntities;
	}
	/**
	 * @brief Create a new 3D rendering window
	 * @param[out] window Pointer to store the new GL window interface
	 * @param[out] widget Pointer to store the Qt widget
	 */
	void createGLWindow(ccGLWindowInterface*& window, QWidget*& widget) const override;
	/**
	 * @brief Destroy a 3D rendering window
	 * @param[in] window The window to destroy
	 */
	void destroyGLWindow(ccGLWindowInterface*) const override;
	/**
	 * @brief Get the unique ID generator for entities
	 * @return Shared pointer to the unique ID generator
	 */
	ccUniqueIDGenerator::Shared getUniqueIDGenerator() override;
	/**
	 * @brief Get the color scales manager
	 * @return Pointer to the color scales manager singleton
	 */
	ccColorScalesManager* getColorScalesManager() override;
	/**
	 * @brief Display a histogram dialog
	 * @param[in] histoValues Histogram data (bin counts)
	 * @param[in] minVal Minimum value for the x-axis
	 * @param[in] maxVal Maximum value for the x-axis
	 * @param[in] title Dialog window title
	 * @param[in] xAxisLabel Label for the x-axis
	 */
	void spawnHistogramDialog(const std::vector<unsigned>& histoValues,
	                                                 double                       minVal,
	                                                 double                       maxVal,
	                                                 QString                      title,
	                                                 QString                      xAxisLabel) override;
	ccPickingHub*               pickingHub() override
	{
		return m_pickingHub;
	}
	/**
	 * @brief Temporarily remove an entity from the DB tree
	 *
	 * Used during drag-and-drop or when an entity needs to be hidden
	 * temporarily without losing its tree state.
	 *
	 * @param[in] obj The entity to remove
	 * @return Context object needed to restore the entity later
	 * @see putObjectBackIntoDBTree()
	 */
	ccHObjectContext removeObjectTemporarilyFromDBTree(ccHObject* obj) override;
	/**
	 * @brief Restore an entity to the DB tree
	 * @param[in] obj The entity to restore
	 * @param[in] context Context from removeObjectTemporarilyFromDBTree()
	 * @see removeObjectTemporarilyFromDBTree()
	 */
	void putObjectBackIntoDBTree(ccHObject* obj, const ccHObjectContext& context) override;

	/**
	 * @brief Handle picked item from 3D view
	 * @param[in] pi Information about the picked item
	 */
	void onItemPicked(const PickedItem& pi) override;

	/**
	 * @brief Get top-level selected entities only
	 *
	 * Unlike getSelectedEntities(), this filters out any entities that are
	 * children of another selected entity, returning only the highest-level
	 * selected items in the hierarchy.
	 *
	 * @return Container of selected top-level entity pointers
	 */
	ccHObject::Container getTopLevelSelectedEntities() const;

	/**
	 * @brief Get the database tree widget
	 * @return Pointer to the ccDBRoot object managing the DB tree
	 */
	virtual ccDBRoot* db();

	/**
	 * @brief Add "Edit Plane" action to a menu
	 *
	 * Adds the plane editing action to the specified menu.
	 * This is the only MainWindow UI action used externally (by ccDBRoot).
	 *
	 * @param[in,out] menu The menu to add the action to
	 */
	void addEditPlaneAction(QMenu& menu) const;

	/**
	 * @brief Initialize plugins
	 *
	 * Sets up menus, toolbars, and other UI elements provided by
	 * loaded plugins. Called after the main window is created.
	 */
	void initPlugins();

	/**
	 * @brief Update the Properties panel
	 *
	 * Refreshes the properties dock widget to show information
	 * about the currently selected entity.
	 */
	void updatePropertiesView() override;

  private:
	/**
	 * @brief Create a new 3D view window
	 * @return Pointer to the new 3D view
	 */
	ccGLWindowInterface* new3DView()
	{
		return new3DViewInternal(true, false);
	}
	//! Creates a new 3D GL sub-window (choose whether entity selection is allowed or not)
	ccGLWindowInterface* new3DViewInternal(bool allowEntitySelection, bool warnAboutLockedRotationAxis = false);

	//! Zooms in (current 3D view)
	void zoomIn();
	//! Zooms out (current 3D view)
	void zoomOut();

	//! Displays the 'help' dialog
	void doActionShowHelpDialog();
	//! Loads one or several files
	void doActionLoadFile();
	//! Save the currently selected entities
	void doActionSaveFile();
	//! Save all the entities at once, BIN format forced
	void doActionSaveProject();
	//! Displays the Global Shift settings dialog
	void doActionGlobalShiftSeetings();
	//! Toggles the 'show Qt warnings in Console' option
	void doEnableQtWarnings(bool);

	//! Clones currently selected entities
	void doActionClone();

	/**
	 * @brief Prepare for window deletion
	 *
	 * Called before a GL window is destroyed to update entities
	 * that may be displaying in it.
	 *
	 * @param[in] glWindow The window being deleted
	 */
	void prepareWindowDeletion(ccGLWindowInterface* glWindow);

	//! Slot called when the exclusive fullscreen mode is toggled on a window
	void onExclusiveFullScreenToggled(bool);

	// inherited from ccMainAppInterface
	void freezeUI(bool state) override;
	void redrawAll(bool only2D = false) override;
	void refreshAll(bool only2D = false) override;
	void enableAll() override;
	void disableAll() override;
	void disableAllBut(ccGLWindowInterface* win) override;
	void updateUI() override;

	virtual void toggleActiveWindowStereoVision(bool);
	void         toggleActiveWindowCenteredPerspective() override;
	void         toggleActiveWindowCustomLight() override;
	void         toggleActiveWindowSunLight() override;
	void         toggleActiveWindowViewerBasedPerspective() override;
	void         zoomOnSelectedEntities() override;
	void         setGlobalZoom() override;

	void increasePointSize() override;
	void decreasePointSize() override;

	void setCustomLightPosition();
	void toggleLockRotationAxis();
	void doActionEnableBubbleViewMode();
	void setPivotAlwaysOn();
	void setPivotRotationOnly();
	void setPivotOff();
	void toggleActiveWindowAutoPickRotCenter(bool);
	void toggleActiveWindowShowCursorCoords(bool);

	//! Handles new label
	void handleNewLabel(ccHObject*);

	void setActiveSubWindow(QWidget* window);
	void showDisplaySettings();
	void showSelectedEntitiesHistogram();
	void testFrameRate();
	void toggleFullScreen(bool state);
	void toggleVisualDebugTraces();
	void toggleExclusiveFullScreen(bool state);
	void update3DViewsMenu();
	void updateMenus();
	void on3DViewActivated(QMdiSubWindow*);
	void updateUIWithSelection();
	void addToDBAuto(const QStringList& filenames);

	void echoMouseWheelRotate(float);
	void echoBaseViewMatRotation(const ccGLMatrixd& rotMat);
	void echoCameraPosChanged(const CCVector3d&);
	void echoPivotPointChanged(const CCVector3d&);

	void doActionRenderToFile();

	// menu action
	void doActionSetUniqueColor();
	void doActionColorize();
	void doActionRGBToGreyScale();
	void doActionSetColor(bool colorize);
	void doActionSetColorGradient();
	void doActionInterpolateColors();
	void doActionChangeColorLevels();
	void doActionEnhanceRGBWithIntensities();
	void doActionColorFromScalars();
	void doActionRGBGaussianFilter();
	void doActionRGBBilateralFilter();
	void doActionRGBMeanFilter();
	void doActionRGBMedianFilter();

	void doActionSFGaussianFilter();
	void doActionSFBilateralFilter();
	void doActionSFConvertToRGB();
	void doActionSFConvertToRandomRGB();
	void doActionRenameSF();
	void doActionOpenSelectedEntitiesSFManager();
	void doActionOpenColorScalesManager();
	void doActionAddIdField();
	void doActionSplitCloudUsingSF();
	void doActionSetSFAsCoord();
	void doActionInterpolateScalarFields();

	void doComputeGeometricFeature();
	void doActionSFGradient();
	void doRemoveDuplicatePoints();
	void doSphericalNeighbourhoodExtractionTest();   // DGM TODO: remove after test
	void doCylindricalNeighbourhoodExtractionTest(); // DGM TODO: remove after test
	void doActionFitPlane();
	void doActionFitSphere();
	void doActionFitCircle();
	void doActionFitFacet();
	void doActionFitQuadric();
	void doShowPrimitiveFactory();

	void doActionComputeNormals();
	void doActionInvertNormals();
	void doActionConvertNormalsToHSV();
	void doActionConvertNormalsToDipDir();
	void doActionComputeOctree();
	void doActionComputeKdTree();
	void doActionApplyTransformation();
	void doActionMerge();
	void doActionRegister();
	void doAction4pcsRegister(); // Aurelien BEY le 13/11/2008
	void doActionSubsample();    // Aurelien BEY le 4/12/2008
	void doActionStatisticalTest();
	void doActionSamplePointsOnMesh();
	void doActionSamplePointsOnPolyline();
	void doActionExtrudePolyline();
	void doActionSmoohPolyline();
	void doActionConvertTextureToColor();
	void doActionLabelConnectedComponents();
	void doActionComputeStatParams();
	void doActionFilterByValue();

	// Picking operations
	void enablePickingOperation(ccGLWindowInterface* win, QString message);
	void cancelPreviousPickingOperation(bool aborted);

	// For rotation center picking
	void doPickRotationCenter();
	// For leveling
	void doLevel();

	void doActionCreatePlane();
	void doActionEditPlane();
	void doActionFlipPlane();
	void doActionComparePlanes();

	void doActionPromoteCircleToCylinder();

	void doActionDeleteScanGrids();
	void doActionSmoothMeshSF();
	void doActionEnhanceMeshSF();
	void doActionAddConstantSF();
	void doActionAddClassificationSF();
	void doActionScalarFieldArithmetic();
	void doActionScalarFieldFromColor();
	void doActionOrientNormalsFM();
	void doActionOrientNormalsMST();
	void doActionShiftPointsAlongNormals();
	void doActionResampleWithOctree();
	void doActionComputeMeshAA();
	void doActionComputeMeshLS();
	void doActionMeshScanGrids();
	void doActionComputeDistanceMap();
	void doActionComputeDistToBestFitQuadric3D();
	void doActionMeasureMeshSurface();
	void doActionMeasureMeshVolume();
	void doActionFlagMeshVertices();
	void doActionSmoothMeshLaplacian();
	void doActionSubdivideMesh();
	void doActionFlipMeshTriangles();
	void doActionComputeCPS();
	void doActionShowWaveDialog();
	void doActionCompressFWFData();
	void doActionKMeans();
	void doActionFrontPropagation();
	void doActionApplyScale();
	void doActionEditGlobalShiftAndScale();
	void doActionMatchBBCenters();
	void doActionMatchScales();
	void doActionSORFilter();
	void doActionFilterNoise();
	void doActionUnroll();
	void doActionCreateGBLSensor();
	void doActionCreateCameraSensor();
	void doActionModifySensor();
	void doActionProjectUncertainty();
	void doActionCheckPointsInsideFrustum();
	void doActionComputeDistancesFromSensor();
	void doActionComputeScatteringAngles();
	void doActionSetViewFromSensor();
	void doActionShowDepthBuffer();
	void doActionExportDepthBuffer();
	void doActionComputePointsVisibility();
	void doActionRasterize();
	void doCompute2HalfDimVolume();
	void doConvertPolylinesToMesh();
	void doMeshTwoPolylines();
	void doActionExportCoordToSF();
	void doActionExportNormalToSF();
	void doActionSetSFsAsNormal();
	void doComputeBestFitBB();
	void doActionCrop();

	void doActionEditCamera();
	void doActionAdjustZoom();
	void doActionShowCurrent3DViewInfo();
	void doActionSaveViewportAsCamera();
	void doActionResetGUIElementsPos();
	void doActionToggleRestoreWindowOnStartup(bool);
	void doActionResetAllVBOs();

	// Shaders & plugins
	void doActionLoadShader();
	void doActionDeleteShader();

	void doActionFindBiggestInnerRectangle();

	// Clipping box
	void activateClippingBoxMode();
	void deactivateClippingBoxMode(bool);

	// Graphical transformation
	void activateTranslateRotateMode();
	void deactivateTranslateRotateMode(bool);

	// Graphical segmentation
	void activateSegmentationMode();
	void deactivateSegmentationMode(bool);

	// Polyline tracing
	void activateTracePolylineMode();
	void deactivateTracePolylineMode(bool);

	// Section extraction
	void activateSectionExtractionMode();
	void deactivateSectionExtractionMode(bool);

	// Entities comparison
	void doActionCloudCloudDist();
	void doActionCloudMeshDist();
	void doActionCloudPrimitiveDist();
	void deactivateComparisonMode(int);

	// Point picking mechanism
	void activatePointPickingMode();
	void deactivatePointPickingMode(bool);

	// Point list picking mechanism
	void activatePointListPickingMode();
	void deactivatePointListPickingMode(bool);

	// Point-pair registration mechanism
	void activateRegisterPointPairTool();
	void deactivateRegisterPointPairTool(bool);

	// Current active scalar field
	void doActionToggleActiveSFColorScale();
	void doActionShowActiveSFPrevious();
	void doActionShowActiveSFNext();

	//! Toggles the clipping planes
	void toggleClippingPlanes();

	//! Removes all entities currently loaded in the DB tree
	void closeAll();

	//! Batch export some info from a set of selected clouds
	void doActionExportCloudInfo();
	//! Batch export some info from a set of selected planes
	void doActionExportPlaneInfo();

	//! Generates a matrix with the best (registration) RMS for all possible couple among the selected entities
	void doActionComputeBestICPRmsMatrix();

	//! Creates a cloud with the (bounding-box) centers of all selected entities
	void doActionCreateCloudFromEntCenters();

	//! Creates a cloud with a single point
	void createSinglePointCloud();
	//! Creates a cloud from the clipboard (ASCII) data
	void createPointCloudFromClipboard();

	inline void doActionMoveBBCenterToOrigin()
	{
		doActionFastRegistration(MoveBBCenterToOrigin);
	}
	inline void doActionMoveBBMinCornerToOrigin()
	{
		doActionFastRegistration(MoveBBMinCornerToOrigin);
	}
	inline void doActionMoveBBMaxCornerToOrigin()
	{
		doActionFastRegistration(MoveBBMaxCornerToOrigin);
	}

	//! Restores position and state of all GUI elements
	void restoreGUIElementsPos();

  private:
	//! Shortcut: asks the user to select one cloud
	/** \param defaultCloudEntity a cloud to select by default (optional)
	    \param inviteMessage invite message (default is something like 'Please select an entity:') (optional)
	    \return the selected cloud (or null if the user cancelled the operation)
	**/
	ccPointCloud* askUserToSelectACloud(ccHObject* defaultCloudEntity = nullptr, QString inviteMessage = QString());

	enum FastRegistrationMode
	{
		MoveBBCenterToOrigin,
		MoveBBMinCornerToOrigin,
		MoveBBMaxCornerToOrigin
	};

	void doActionFastRegistration(FastRegistrationMode mode);

	void toggleSelectedEntitiesProperty(ccEntityAction::TOGGLE_PROPERTY property);
	void clearSelectedEntitiesProperty(ccEntityAction::CLEAR_PROPERTY property);

	void setView(CC_VIEW_ORIENTATION view) override;

	//! Apply transformation to the selected entities
	void applyTransformation(const ccGLMatrixd& transMat, bool applyToGlobal);

	//! Creates point clouds from multiple 'components'
	void createComponentsClouds(ccGenericPointCloud*                cloud,
	                            CCCoreLib::ReferenceCloudContainer& components,
	                            unsigned                            minPointPerComponent,
	                            bool                                randomColors,
	                            bool                                selectComponents,
	                            bool                                sortBysize = true);

	//! Saves position and state of all GUI elements
	void saveGUIElementsPos();

	void setOrthoView(ccGLWindowInterface* win);
	void setCenteredPerspectiveView(ccGLWindowInterface* win, bool autoRedraw = true);
	void setViewerPerspectiveView(ccGLWindowInterface* win);

	void showEvent(QShowEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	void moveEvent(QMoveEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

	//! Makes the window including an entity zoom on it (helper)
	void zoomOn(ccHObject* object);

	//! Active SF action fork
	/** - action=0 : toggle SF color scale
	    - action=1 : activate previous SF
	    - action=2 : activate next SF
	    \param action action id
	**/
	void doApplyActiveSFAction(int action);

	//! Mesh computation fork
	/** \param type triangulation type
	 **/
	void doActionComputeMesh(CCCoreLib::TRIANGULATION_TYPES type);

	//! Computes the orientation of an entity
	/** Either fit a plane or a 'facet' (2D polygon)
	 **/
	void doComputePlaneOrientation(bool fitFacet);

	//! Sets up any input devices (3D mouse, gamepad) and adds their menus
	void setupInputDevices();
	//! Stops input and destroys any input device handling
	void destroyInputDevices();

	//! Connects all QT actions to slots
	void connectActions();

	//! Enables menu entires based on the current selection
	void enableUIItems(dbTreeSelectionInfo& selInfo);

	//! Updates the view mode pop-menu based for a given window (or an absence of!)
	void updateViewModePopUpMenu(ccGLWindowInterface* win);

	//! Updates the pivot visibility pop-menu based for a given window (or an absence of!)
	void updatePivotVisibilityPopUpMenu(ccGLWindowInterface* win);

	//! Checks whether stereo mode can be stopped (if necessary) or not
	bool checkStereoMode(ccGLWindowInterface* win);

	//! Adds a single value SF to the active point cloud
	void addConstantSF(ccPointCloud* cloud, QString sfName, bool integerValue);

	void populateActionList();
	void showShortcutDialog();

  private: // members
	//! Main UI
	Ui::MainWindow* m_UI;

	//! DB tree
	ccDBRoot* m_ccRoot;

	//! Currently selected entities;
	ccHObject::Container m_selectedEntities;

	//! UI frozen state (see freezeUI)
	bool m_uiFrozen;

	//! Recent files menu
	ccRecentFiles* m_recentFiles;

	//! 3D mouse
	cc3DMouseManager* m_3DMouseManager;

	//! View mode pop-up menu button
	QToolButton* m_viewModePopupButton;

	//! Pivot visibility pop-up menu button
	QToolButton* m_pivotVisibilityPopupButton;

	//! Flag: first time the window is made visible
	bool m_firstShow;

	//! Point picking hub
	ccPickingHub* m_pickingHub;

	/******************************/
	/***        MDI AREA        ***/
	/******************************/

	QMdiArea* m_mdiArea;

	//! CloudCompare MDI area overlay dialogs
	struct ccMDIDialogs
	{
		ccOverlayDialog* dialog;
		Qt::Corner       position;

		//! Constructor with dialog and position
		ccMDIDialogs(ccOverlayDialog* dlg, Qt::Corner pos)
		    : dialog(dlg)
		    , position(pos)
		{
		}
	};

	//! Repositions an MDI dialog at its right position
	void repositionOverlayDialog(ccMDIDialogs& mdiDlg);

	//! Registered MDI area 'overlay' dialogs
	std::vector<ccMDIDialogs> m_mdiDialogs;

	/*** dialogs ***/
	//! Camera params dialog
	ccCameraParamEditDlg* m_cpeDlg;
	//! Graphical segmentation dialog
	ccGraphicalSegmentationTool* m_gsTool;
	//! Polyline tracing tool
	ccTracePolylineTool* m_tplTool;
	//! Section extraction dialog
	ccSectionExtractionTool* m_seTool;
	//! Graphical transformation dialog
	ccGraphicalTransformationTool* m_transTool;
	//! Clipping box dialog
	ccClippingBoxTool* m_clipTool;
	//! Cloud comparison dialog
	ccComparisonDlg* m_compDlg;
	//! Point properties mode dialog
	ccPointPropertiesDlg* m_ppDlg;
	//! Point list picking
	ccPointListPickingDlg* m_plpDlg;
	//! Point-pair registration
	ccPointPairRegistrationDlg* m_pprDlg;
	//! Primitive factory dialog
	ccPrimitiveFactoryDlg* m_pfDlg;
	//! Shortcut management dialog
	ccShortcutDialog* m_shortcutDlg;

	/*** plugins ***/
	//! Manages plugins - menus, toolbars, and the about dialog
	ccPluginUIManager* m_pluginUIManager;

	QList<QAction*> m_actions;
};
