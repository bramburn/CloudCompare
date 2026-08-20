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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPropertiesTreeDelegate.h
 *
 * @brief Properties tree delegate for entity property editing.
 *
 * @details Qt item delegate for displaying and editing object properties
 * in the CloudCompare database tree.
 *
 * Properties handled:
 * - Object name and visibility
 * - Color settings (RGB, scalar field, normals)
 * - Display options (wireframe, stippling)
 * - Sensor parameters
 * - Primitive dimensions
 * - Image settings
 * - Transformation matrices
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_ITEM_DELEGATE_HEADER
#define CC_ITEM_DELEGATE_HEADER

// qCC_db
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
 * @brief Delegate for entity property editing.
 *
 * @details Provides a Qt item delegate that displays and allows
 * editing of entity properties in a tree view.
 *
 * Supports many entity types:
 * - Point clouds
 * - Meshes
 * - Sensors (camera, GBL)
 * - Primitives (spheres, cones, etc.)
 * - Polylines
 * - Images
 * - Labels
 * - Coordinate systems
 *
 * @extends QStyledItemDelegate
 */
class ccPropertiesTreeDelegate : public QStyledItemDelegate
{
	Q_OBJECT

  public:
	/**
	 * @brief Property roles for items.
	 */
	enum CC_PROPERTY_ROLE
	{
		OBJECT_NO_PROPERTY = 0,

		// General
		OBJECT_NAME,
		OBJECT_VISIBILITY,
		OBJECT_CURRENT_DISPLAY,

		// Normals
		OBJECT_NORMALS_SHOWN,
		OBJECT_CLOUD_DRAW_NORMALS,
		OBJECT_CLOUD_NORMAL_COLOR,
		OBJECT_CLOUD_NORMAL_LENGTH,

		// Colors
		OBJECT_COLOR_SOURCE,
		OBJECT_CLOUD_SF_EDITOR,
		OBJECT_CURRENT_SCALAR_FIELD,
		OBJECT_CURRENT_COLOR_RAMP,
		OBJECT_COLOR_RAMP_STEPS,

		// Display
		OBJECT_MESH_WIRE,
		OBJECT_MESH_STIPPLING,
		OBJECT_SF_SHOW_SCALE,

		// Octree
		OBJECT_OCTREE_LEVEL,
		OBJECT_OCTREE_TYPE,

		// Point cloud
		OBJECT_CLOUD_POINT_SIZE,
		OBJECT_CLOUD_USE_LOD,

		// Polyline
		OBJECT_POLYLINE_WIDTH,

		// Sensor
		OBJECT_SENSOR_DRAW_FRUSTUM,
		OBJECT_SENSOR_DRAW_FRUSTUM_PLANES,
		OBJECT_SENSOR_DISPLAY_SCALE,
		OBJECT_SENSOR_UNCERTAINTY,
		OBJECT_SENSOR_INDEX,
		OBJECT_SENSOR_MATRIX_EDITOR,
		OBJECT_APPLY_SENSOR_VIEWPORT,

		// Image
		OBJECT_IMAGE_ALPHA,
		OBJECT_APPLY_IMAGE_VIEWPORT,

		// Primitive
		OBJECT_PRIMITIVE_PRECISION,
		OBJECT_CIRCLE_RESOLUTION,
		OBJECT_SPHERE_RADIUS,
		OBJECT_CIRCLE_RADIUS,
		OBJECT_DISC_RADIUS,
		OBJECT_CONE_HEIGHT,
		OBJECT_CONE_BOTTOM_RADIUS,
		OBJECT_CONE_TOP_RADIUS,

		// Facet
		OBJECT_FACET_CONTOUR,
		OBJECT_FACET_MESH,

		// Plane
		OBJECT_PLANE_NORMAL_VECTOR,

		// Material
		OBJECT_MATERIALS,

		// Label
		OBJECT_APPLY_LABEL_VIEWPORT,
		OBJECT_UPDATE_LABEL_VIEWPORT,
		OBJECT_LABEL_DISP_2D,
		OBJECT_LABEL_POINT_LEGEND,

		// Transformation
		OBJECT_SHOW_TRANS_BUFFER_PATH,
		OBJECT_SHOW_TRANS_BUFFER_TRIHDERONS,
		OBJECT_TRANS_BUFFER_TRIHDERONS_SCALE,
		OBJECT_HISTORY_MATRIX_EDITOR,
		OBJECT_GLTRANS_MATRIX_EDITOR,

		// Coordinate system
		OBJECT_COORDINATE_SYSTEM_DISP_PLANES,
		OBJECT_COORDINATE_SYSTEM_DISP_AXES,
		OBJECT_COORDINATE_SYSTEM_AXES_WIDTH,
		OBJECT_COORDINATE_SYSTEM_DISP_SCALE,

		// Name in 3D
		OBJECT_NAME_IN_3D,

		// Octree
		OBJECT_SENSOR_INDEX,

		// Header
		TREE_VIEW_HEADER,
	};

	/**
	 * @brief Construct the delegate.
	 *
	 * @param[in] _model Item model.
	 * @param[in] _view Item view.
	 * @param[in] parent Parent object.
	 */
	ccPropertiesTreeDelegate(QStandardItemModel* _model, QAbstractItemView* _view, QObject* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccPropertiesTreeDelegate() override;

	// inherited from QStyledItemDelegate
	/**
	 * @brief Get size hint.
	 */
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/**
	 * @brief Create editor widget.
	 */
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/**
	 * @brief Update editor geometry.
	 */
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/**
	 * @brief Set editor data from model.
	 */
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;

	/**
	 * @brief Unbind from current object.
	 */
	void unbind();

	/**
	 * @brief Fill model with object properties.
	 *
	 * @param[in] hObject Object to display.
	 */
	void fillModel(ccHObject* hObject);

	/**
	 * @brief Get currently bound object.
	 *
	 * @return Current object.
	 */
	ccHObject* getCurrentObject();

  signals:
	/**
	 * @brief Emitted when object properties change.
	 *
	 * @param[in] hObject Changed object.
	 */
	void ccObjectPropertiesChanged(ccHObject* hObject) const;

	/**
	 * @brief Emitted when object appearance changes.
	 *
	 * @param[in] hObject Changed object.
	 */
	void ccObjectAppearanceChanged(ccHObject* hObject) const;

	/**
	 * @brief Emitted when object and children appearance changes.
	 *
	 * @param[in] hObject Changed object.
	 */
	void ccObjectAndChildrenAppearanceChanged(ccHObject* hObject) const;

  private:
	static const char* s_noneString;
	static const char* s_rgbColor;
	static const char* s_sfColor;
	static const char* s_defaultPointSizeString;
	static const char* s_defaultPolyWidthSizeString;

	void updateItem(QStandardItem*);
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

	void addSeparator(const QString& title);
	void appendRow(QStandardItem* leftItem, QStandardItem* rightItem, bool openPersistentEditor = false);
	void appendWideRow(QStandardItem* item, bool openPersistentEditor = true);

	void fillWithHObject(ccHObject*);
	void fillWithPointCloud(ccGenericPointCloud*);
	void fillWithDrawNormals(ccGenericPointCloud*);
	void fillWithPointCloudLOD(ccGenericPointCloud*);
	void fillSFWithPointCloud(ccGenericPointCloud*);
	void fillWithMesh(const ccGenericMesh*);
	void fillWithFacet(const ccFacet*);
	void fillWithPlanarEntity(const ccPlanarEntityInterface*);
	void fillWithSensor(const ccSensor*);
	void fillWithTransBuffer(const ccIndexedTransformationBuffer*);
	void fillWithPolyline(const ccPolyline*);
	void fillWithPrimitive(const ccGenericPrimitive*);
	void fillWithPointOctree(const ccOctree*);
	void fillWithPointKdTree(const ccKdTree*);
	void fillWithImage(const ccImage*);
	void fillWithLabel(const cc2DLabel*);
	void fillWithViewportObject(const cc2DViewportObject*);
	void fillWithGBLSensor(const ccGBLSensor*);
	void fillWithCameraSensor(const ccCameraSensor*);
	void fillWithMaterialSet(const ccMaterialSet*);
	void fillWithShareable(const CCShareable*);
	void fillWithMetaData(const ccObject*);
	void fillWithShifted(const ccShiftedObject*);
	void fillWithCoordinateSystem(const ccCoordinateSystem*);

	template <class Type, int N, class ComponentType>
	void fillWithCCArray(const ccArray<Type, N, ComponentType>*);

	/**
	 * @brief Check if editor is wide.
	 *
	 * @param[in] itemData Property role.
	 *
	 * @return true if wide (two columns).
	 */
	bool isWideEditor(int itemData) const;

	/**
	 * @brief Update model for same object.
	 */
	void updateModel();

  private:
	//! Current object
	ccHObject* m_currentObject;

	//! Item model
	QStandardItemModel* m_model;

	//! Item view
	QAbstractItemView* m_view;

	//! Last focused item role
	CC_PROPERTY_ROLE m_lastFocusItemRole;
};

#endif
