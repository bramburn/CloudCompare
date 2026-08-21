// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_ITEM_DELEGATE_HEADER
#define CC_ITEM_DELEGATE_HEADER

/**
 * @file ccPropertiesTreeDelegate.h
 *
 * @brief Qt Model/View delegate for the object properties panel
 *
 * Implements a QStyledItemDelegate that populates the right-side
 * Properties panel in CloudCompare's UI with editable properties
 * for the currently selected object.
 *
 * ## Architecture
 *
 * The delegate works with the Qt Model/View pattern:
 * - Model: QStandardItemModel (m_model)
 * - View: QAbstractItemView (m_view)
 * - Delegate: ccPropertiesTreeDelegate (this class)
 *
 * When an object is selected, fillModel() populates the tree
 * with property rows specific to that object type.
 *
 * ## Property Categories
 *
 * Each object type contributes its own property rows:
 * - **ccHObject**: name, visibility, color source, display
 * - **ccGenericPointCloud**: scalar field, point size, normals, LOD
 * - **ccGenericMesh**: wire mode, stippling, materials
 * - **ccPolyline**: width
 * - **ccSensor**: frustum, scale, uncertainty, matrix
 * - **ccImage**: alpha, viewport
 * - **ccFacet**: contour, mesh, normal
 * - **ccGenericPrimitive**: precision, radius, height
 * - **ccCoordinateSystem**: display scale, axes width
 * - **cc2DLabel**: 2D label display, point legend
 * - **ccGBLSensor / ccCameraSensor**: sensor-specific display
 *
 * ## CC_PROPERTY_ROLE
 *
 * Enumerates every editable property in the panel. Used to
 * identify which property changed and respond appropriately.
 * Values range from OBJECT_NO_PROPERTY (0) to
 * OBJECT_CLOUD_DRAW_NORMALS (~67).
 *
 * ## Signal Flow
 *
 * User edits a property → setEditorData() reads the widget value →
 * commitData() signal → slot handles change → emits
 * ccObjectPropertiesChanged() or ccObjectAppearanceChanged().
 */

#include <ccArray.h>
#include <ccPointCloud.h>

// Qt
#include <QStyledItemDelegate>

class cc2DLabel;
class cc2DViewportObject;
class ccCameraSensor;
class ccFacet;
class ccGBLSensor;
class ccGenericMesh;
class ccGenericPointCloud;
class ccGenericPrimitive;
class ccHObject;
class ccImage;
class ccIndexedTransformationBuffer;
class ccKdTree;
class ccMaterialSet;
class ccOctree;
class ccPlanarEntityInterface;
class ccPolyline;
class ccSensor;
class CCShareable;
class ccShiftedObject;
class ccCoordinateSystem;

class QAbstractItemView;
class QStandardItem;
class QStandardItemModel;

/**
 * @class ccPropertiesTreeDelegate
 *
 * @brief Qt delegate for the object properties panel
 *
 * Populates and manages the properties tree (right panel) in the
 * CloudCompare UI. Provides inline editors for object properties.
 *
 * ## Signal Flow
 *
 * User edits → slot → ccObjectPropertiesChanged() / ccObjectAppearanceChanged()
 *
 * ## Role System
 *
 * Each row has a CC_PROPERTY_ROLE that identifies the property type.
 * This role is stored in the QStandardItem's data() and used by
 * slot handlers to determine what to update.
 *
 * @extends QStyledItemDelegate
 */
class ccPropertiesTreeDelegate : public QStyledItemDelegate
{
	Q_OBJECT

  public:
	/**
	 * @brief Property role identifiers
	 *
	 * Identifies every editable property in the properties panel.
	 * Used to route slot handlers and distinguish between
	 * property changes and appearance changes.
	 */
	enum CC_PROPERTY_ROLE
	{
		OBJECT_NO_PROPERTY = 0,
		OBJECT_NAME,                 //!< Object display name
		OBJECT_VISIBILITY,           //!< Visible/hidden
		OBJECT_CURRENT_DISPLAY,      //!< Active 3D view
		OBJECT_NORMALS_SHOWN,       //!< Show/hide normals
		OBJECT_COLOR_SOURCE,         //!< Color source (default, from SF, etc.)
		OBJECT_POLYLINE_WIDTH,      //!< Polyline display width
		OBJECT_SENSOR_DRAW_FRUSTUM, //!< Draw sensor frustum
		OBJECT_SENSOR_DRAW_FRUSTUM_PLANES, //!< Draw frustum clipping planes
		OBJECT_SF_SHOW_SCALE,       //!< Show scalar field color scale bar
		OBJECT_OCTREE_LEVEL,        //!< Octree display level
		OBJECT_OCTREE_TYPE,         //!< Octree display mode
		OBJECT_MESH_WIRE,           //!< Wireframe mode
		OBJECT_MESH_STIPPLING,      //!< Stippled rendering
		OBJECT_CURRENT_SCALAR_FIELD,//!< Active scalar field
		OBJECT_CURRENT_COLOR_RAMP, //!< Active color ramp
		OBJECT_IMAGE_ALPHA,         //!< Image transparency
		OBJECT_APPLY_IMAGE_VIEWPORT,//!< Apply image to viewport
		OBJECT_APPLY_SENSOR_VIEWPORT,//!< Apply sensor as viewport
		OBJECT_CLOUD_SF_EDITOR,     //!< Scalar field editor
		OBJECT_SENSOR_MATRIX_EDITOR,//!< Sensor transformation matrix
		OBJECT_SENSOR_DISPLAY_SCALE,//!< Sensor display scale
		OBJECT_SENSOR_UNCERTAINTY,  //!< Sensor uncertainty
		OBJECT_COLOR_RAMP_STEPS,    //!< Color ramp step count
		OBJECT_MATERIALS,           //!< Material list
		OBJECT_APPLY_LABEL_VIEWPORT,//!< Apply label to viewport
		OBJECT_UPDATE_LABEL_VIEWPORT,//!< Update label viewport
		OBJECT_LABEL_DISP_2D,       //!< 2D label display
		OBJECT_LABEL_POINT_LEGEND,  //!< Point legend
		OBJECT_PRIMITIVE_PRECISION, //!< Primitive tessellation precision
		OBJECT_CIRCLE_RESOLUTION,  //!< Circle tessellation
		OBJECT_SPHERE_RADIUS,       //!< Sphere radius
		OBJECT_CIRCLE_RADIUS,       //!< Circle radius
		OBJECT_DISC_RADIUS,         //!< Disc radius
		OBJECT_CONE_HEIGHT,         //!< Cone height
		OBJECT_CONE_BOTTOM_RADIUS,  //!< Cone bottom radius
		OBJECT_CONE_TOP_RADIUS,     //!< Cone top radius
		OBJECT_CLOUD_POINT_SIZE,   //!< Point display size
		OBJECT_NAME_IN_3D,          //!< 3D name label
		OBJECT_FACET_CONTOUR,       //!< Facet contour polyline
		OBJECT_FACET_MESH,          //!< Facet mesh
		OBJECT_PLANE_NORMAL_VECTOR, //!< Plane normal display
		OBJECT_SENSOR_INDEX,        //!< Sensor index
		OBJECT_SHOW_TRANS_BUFFER_PATH,    //!< Transformation buffer path
		OBJECT_SHOW_TRANS_BUFFER_TRIHDERONS,//!< Show transformation buffer trihedrons
		OBJECT_TRANS_BUFFER_TRIHDERONS_SCALE,//!< Trihedron scale
		OBJECT_HISTORY_MATRIX_EDITOR,//!< Transformation history
		OBJECT_GLTRANS_MATRIX_EDITOR,//!< GL transformation matrix
		OBJECT_COORDINATE_SYSTEM_DISP_PLANES, //!< Coordinate system display planes
		OBJECT_COORDINATE_SYSTEM_DISP_AXES,   //!< Coordinate system axes
		OBJECT_COORDINATE_SYSTEM_AXES_WIDTH,  //!< Axis line width
		OBJECT_COORDINATE_SYSTEM_DISP_SCALE,  //!< Coordinate system scale
		TREE_VIEW_HEADER,             //!< Tree view header
		OBJECT_CLOUD_NORMAL_COLOR,   //!< Normal vector color
		OBJECT_CLOUD_NORMAL_LENGTH,   //!< Normal vector length
		OBJECT_CLOUD_DRAW_NORMALS,   //!< Draw point normals
		OBJECT_CLOUD_USE_LOD,        //!< Use level-of-detail
	};

	/**
	 * @brief Construct the delegate
	 *
	 * @param[in] _model Standard item model backing the properties tree
	 * @param[in] _view The properties tree view
	 * @param[in] parent Parent widget
	 */
	ccPropertiesTreeDelegate(QStandardItemModel* _model, QAbstractItemView* _view, QObject* parent = nullptr);

	//! Destructor
	~ccPropertiesTreeDelegate() override;

	// QStyledItemDelegate

	/**
	 * @brief Compute the size of a property row
	 *
	 * @param[in] option Style options
	 * @param[in] index Model index
	 * @return Size hint for the row
	 */
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/**
	 * @brief Create the appropriate editor widget for a property
	 *
	 * Dispatches to the right widget type based on the property role:
	 * - Spinboxes for numeric values (int, double)
	 * - Color picker for colors
	 * - Combo box for enums
	 * - Checkbox for booleans
	 *
	 * @param[in] parent Parent widget for the editor
	 * @param[in] option Style options
	 * @param[in] index Model index
	 * @return Editor widget (or nullptr)
	 */
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/**
	 * @brief Position the editor in the view
	 */
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/**
	 * @brief Transfer data from model to editor widget
	 */
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;

	/**
	 * @brief Detach from the current object
	 *
	 * Clears the model and unbinds the current object.
	 */
	void unbind();

	/**
	 * @brief Populate the properties tree for an object
	 *
	 * Clears the model and dispatches to the appropriate fillWith*()
	 * method based on the object's class type.
	 *
	 * @param[in] hObject Object whose properties to display
	 */
	void fillModel(ccHObject* hObject);

	/**
	 * @brief Get the currently bound object
	 */
	ccHObject* getCurrentObject();

  signals:
	/**
	 * @brief Emitted when an object's core properties change
	 *
	 * @param[in] hObject The affected object
	 */
	void ccObjectPropertiesChanged(ccHObject* hObject) const;

	/**
	 * @brief Emitted when an object's appearance changes
	 *
	 * @param[in] hObject The affected object
	 */
	void ccObjectAppearanceChanged(ccHObject* hObject) const;

	/**
	 * @brief Emitted when an object and its children change appearance
	 *
	 * @param[in] hObject The root affected object
	 */
	void ccObjectAndChildrenAppearanceChanged(ccHObject* hObject) const;

  private:
	//! String constants for display
	static const char* s_noneString;
	static const char* s_rgbColor;
	static const char* s_sfColor;
	static const char* s_defaultPointSizeString;
	static const char* s_defaultPolyWidthSizeString;

	//! Update a single property item
	void updateItem(QStandardItem*);

	// Property-specific change handlers
	///@{
	void scalarFieldChanged(int);
	void colorScaleChanged(int);
	void colorRampStepsChanged(int);
	void spawnColorRampEditor();
	void octreeDisplayModeChanged(int);
	void octreeDisplayedLevelChanged(int);
	void primitivePrecisionChanged(int);
	void circleResolutionChanged(int);
	void sphereRadiusChanged(double);
	void circleRadiusChanged(double);
	void discRadiusChanged(double);
	void coneHeightChanged(double);
	void coneBottomRadiusChanged(double);
	void coneTopRadiusChanged(double);
	void imageAlphaChanged(int);
	void applyImageViewport();
	void applySensorViewport();
	void applyLabelViewport();
	void updateLabelViewport();
	void updateDisplay();
	void objectDisplayChanged(const QString&);
	void colorSourceChanged(const QString&);
	void sensorScaleChanged(double);
	void coordinateSystemDisplayScaleChanged(double);
	void sensorUncertaintyChanged();
	void sensorIndexChanged(double);
	void cloudPointSizeChanged(int);
	void polyineWidthChanged(int);
	void coordinateSystemAxisWidthChanged(int);
	void trihedronsScaleChanged(double);
	void normalColorChanged(int colorIdx);
	void normalLengthChanged(double length);
	///@}

	//! Add a section separator
	void addSeparator(const QString& title);

	//! Add a two-column property row
	void appendRow(QStandardItem* leftItem, QStandardItem* rightItem, bool openPersistentEditor = false);

	//! Add a wide (single-column) property row
	void appendWideRow(QStandardItem* item, bool openPersistentEditor = true);

	//! Populate with base ccHObject properties
	void fillWithHObject(ccHObject*);
	//! Populate with point cloud properties
	void fillWithPointCloud(ccGenericPointCloud*);
	//! Populate with point cloud normal properties
	void fillWithDrawNormals(ccGenericPointCloud*);
	//! Populate with point cloud LOD properties
	void fillWithPointCloudLOD(ccGenericPointCloud*);
	//! Populate scalar field properties
	void fillSFWithPointCloud(ccGenericPointCloud*);
	//! Populate with mesh properties
	void fillWithMesh(const ccGenericMesh*);
	//! Populate with facet properties
	void fillWithFacet(const ccFacet*);
	//! Populate with planar entity (plane) properties
	void fillWithPlanarEntity(const ccPlanarEntityInterface*);
	//! Populate with sensor properties
	void fillWithSensor(const ccSensor*);
	//! Populate with transformation buffer properties
	void fillWithTransBuffer(const ccIndexedTransformationBuffer*);
	//! Populate with polyline properties
	void fillWithPolyline(const ccPolyline*);
	//! Populate with primitive properties
	void fillWithPrimitive(const ccGenericPrimitive*);
	//! Populate with octree display properties
	void fillWithPointOctree(const ccOctree*);
	//! Populate with kd-tree display properties
	void fillWithPointKdTree(const ccKdTree*);
	//! Populate with image properties
	void fillWithImage(const ccImage*);
	//! Populate with label properties
	void fillWithLabel(const cc2DLabel*);
	//! Populate with viewport object properties
	void fillWithViewportObject(const cc2DViewportObject*);
	//! Populate with GBL sensor properties
	void fillWithGBLSensor(const ccGBLSensor*);
	//! Populate with camera sensor properties
	void fillWithCameraSensor(const ccCameraSensor*);
	//! Populate with material set properties
	void fillWithMaterialSet(const ccMaterialSet*);
	//! Populate with shareable object properties
	void fillWithShareable(const CCShareable*);
	//! Populate with metadata properties
	void fillWithMetaData(const ccObject*);
	//! Populate with shifted object properties
	void fillWithShifted(const ccShiftedObject*);
	//! Populate with coordinate system properties
	void fillWithCoordinateSystem(const ccCoordinateSystem*);

	//! Fill with ccArray-derived class
	template <class Type, int N, class ComponentType>
	void fillWithCCArray(const ccArray<Type, N, ComponentType>*);

	//! Check if an editor should span both columns
	bool isWideEditor(int itemData) const;

	//! Update the model without rebinding
	void updateModel();

	//! Currently displayed object
	ccHObject*          m_currentObject;
	//! Model backing the properties tree
	QStandardItemModel* m_model;
	//! The properties tree view
	QAbstractItemView*  m_view;
	//! Last focused property role
	CC_PROPERTY_ROLE    m_lastFocusItemRole;
};

#endif
