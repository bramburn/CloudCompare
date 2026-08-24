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
// ##########################################################################

#ifndef CC_DB_ROOT_HEADER
#define CC_DB_ROOT_HEADER

/**
 * @file ccDBRoot.h
 *
 * @brief Database root and tree view model
 *
 * Implements the Qt Model/View architecture for CloudCompare's database tree.
 * The DB tree displays the hierarchical scene graph (ccHObject tree) in a
 * QTreeView, while the properties panel shows editable properties of the
 * currently selected entity.
 *
 * Architecture:
 * - ccDBRoot extends QAbstractItemModel — provides data to the QTreeView
 * - ccCustomQTreeView handles selection behavior
 * - ccPropertiesTreeDelegate provides custom editing for property cells
 * - ccHObject is the underlying data model (scene graph)
 *
 * Signals:
 * - selectionChanged: emitted when the selected set changes
 * - dbIsEmpty / dbIsNotEmptyAnymore: emitted as entities are added/removed
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QAbstractItemModel>
#include <QPoint>
#include <QTreeView>

// qCC_db
#include <ccHObject.h>

// System
#include <unordered_set>

class QAction;
class QStandardItemModel;
class ccPropertiesTreeDelegate;
class ccHObject;

/**
 * @brief Aggregated statistics about the current selection
 *
 * Tallies the number of entities of each type currently selected in the DB tree.
 * Used to enable/disable context menu actions based on selection content.
 */
struct dbTreeSelectionInfo
{
	size_t selCount = 0;          //!< Total selected entities
	size_t sfCount = 0;           //!< Entities with scalar fields
	size_t colorCount = 0;        //!< Entities with colors
	size_t normalsCount = 0;      //!< Entities with normals
	size_t octreeCount = 0;       //!< Octree entities
	size_t cloudCount = 0;        //!< Point clouds
	size_t gridCound = 0;         //!< Grid/sensor entities
	size_t groupCount = 0;        //!< Group containers
	size_t polylineCount = 0;     //!< Polyline entities
	size_t planeCount = 0;        //!< Plane primitives
	size_t circleCount = 0;       //!< Circle/primitive entities
	size_t meshCount = 0;         //!< Mesh entities
	size_t primitiveCount = 0;    //!< Generic primitives
	size_t imageCount = 0;        //!< Image entities
	size_t sensorCount = 0;       //!< Sensor entities
	size_t gblSensorCount = 0;    //!< GBLSensor entities
	size_t cameraSensorCount = 0; //!< Camera sensor entities
	size_t kdTreeCount = 0;       //!< KD-tree entities
};

//! Custom QTreeView with advanced selection behavior
/**
 * @brief Database tree view with custom selection handling
 *
 * Extends QTreeView to implement Ctrl+Click multi-selection semantics
 * and drag-drop reordering of items in the DB tree.
 */
class ccCustomQTreeView : public QTreeView
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the custom tree view
	 * @param[in] parent Parent widget
	 */
	explicit ccCustomQTreeView(QWidget* parent)
	    : QTreeView(parent)
	{
	}

  protected:
	/**
	 * @brief Determines selection behavior for an index
	 *
	 * Overridden to support:
	 * - Ctrl+Click to toggle individual item selection
	 * - Drag to reorder items within the hierarchy
	 *
	 * @param[in] index  Model index under cursor
	 * @param[in] event  Current mouse/key event (may be nullptr)
	 * @return Selection flags for this interaction
	 */
	QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index, const QEvent* event = nullptr) const override;
};

/**
 * @brief Database tree root — Qt Model for the DB hierarchy
 *
 * Implements QAbstractItemModel to present a ccHObject tree as a Qt model.
 * Manages the main database tree view (entity hierarchy) and the properties
 * panel (editable properties of the selected entity).
 *
 * The class bridges:
 * - ccHObject (scene graph data structure) ↔ QAbstractItemModel (Qt views)
 * - QTreeView (DB tree display) ↔ QTreeView (properties panel)
 *
 * Key responsibilities:
 * - Display the entity hierarchy with icons and labels
 * - Handle selection (single, multi, Ctrl+Click)
 * - Context menu actions (delete, toggle visibility, rename, etc.)
 * - Drag-and-drop entity reordering
 * - Properties panel synchronization
 *
 * @see ccHObject, ccCustomQTreeView, ccPropertiesTreeDelegate
 */
class ccDBRoot : public QAbstractItemModel
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the DB root manager
	 *
	 * Sets up the two tree views (DB tree + properties panel) and
	 * populates the DB tree with the root entity.
	 *
	 * @param[in] dbTreeWidget          DB tree QTreeView
	 * @param[in] propertiesTreeWidget  Properties panel QTreeView
	 * @param[in] parent                QObject parent
	 */
	ccDBRoot(ccCustomQTreeView* dbTreeWidget, QTreeView* propertiesTreeWidget, QObject* parent = nullptr);

	/**
	 * @brief Destructor
	 *
	 * Releases all allocated resources including the properties model
	 * and delegate.
	 */
	~ccDBRoot() override;

	/**
	 * @brief Get the underlying scene graph root
	 * @return Pointer to the ccHObject root entity
	 */
	ccHObject* getRootEntity();

	/**
	 * @brief Hide the properties panel
	 */
	void hidePropertiesView();

	/**
	 * @brief Update the properties panel to reflect current selection
	 */
	void updatePropertiesView();

	/**
	 * @brief Add an entity to the DB tree
	 *
	 * Inserts the entity as a child of the root (or its appropriate
	 * parent) and optionally expands the tree to show it.
	 *
	 * @param[in] object     Entity to add
	 * @param[in] autoExpand Expand the parent branch after insertion
	 */
	void addElement(ccHObject* object, bool autoExpand = true);

	/**
	 * @brief Remove an entity from the DB tree
	 *
	 * Removes from the tree and calls prepareDisplayForRefresh()
	 * on the object before deletion.
	 *
	 * @param[in] object Entity to remove
	 */
	void removeElement(ccHObject* object);

	/**
	 * @brief Remove multiple entities at once
	 *
	 * More efficient than multiple removeElement() calls.
	 * Clears the input container after removal.
	 *
	 * @param[in,out] objects Entities to remove (container is cleared)
	 */
	void removeElements(ccHObject::Container& objects);

	/**
	 * @brief Find an entity by unique ID
	 * @param[in] uniqueID Unique identifier to search for
	 * @return Pointer to the entity, or nullptr if not found
	 */
	ccHObject* find(int uniqueID) const;

	/**
	 * @brief Count selected entities of a given type
	 *
	 * @param[in] filter Entity type filter (default: all types)
	 * @return Number of selected entities matching the filter
	 */
	int countSelectedEntities(CC_CLASS_ENUM filter = CC_TYPES::OBJECT);

	/**
	 * @brief Get all selected entities
	 *
	 * @param[out] selectedEntities Populated with selected entity pointers
	 * @param[in]  filter           Entity type filter (default: all types)
	 * @param[out] info             Optional selection statistics (enables/disables context menu actions)
	 * @return Number of entities added to selectedEntities
	 */
	size_t getSelectedEntities(ccHObject::Container& selectedEntities,
	                           CC_CLASS_ENUM filter = CC_TYPES::OBJECT,
	                           dbTreeSelectionInfo* info = nullptr);

	/**
	 * @brief Expand or collapse a tree node
	 * @param[in] object Entity whose node to expand/collapse
	 * @param[in] state  true = expand, false = collapse
	 */
	void expandElement(ccHObject* object, bool state);

	/**
	 * @brief Remove a specific entity from the current selection
	 * @param[in] obj Entity to deselect
	 */
	void unselectEntity(ccHObject* obj);

	/**
	 * @brief Deselect all entities
	 */
	void unselectAllEntities();

	/**
	 * @brief Unload all entities from the DB
	 *
	 * Removes all children from the root entity.
	 */
	void unloadAll();

	// QAbstractItemModel interface

	/**
	 * @brief Get data for a model index
	 *
	 * Returns display/role data for tree columns:
	 * - Column 0: entity name with visibility icon
	 * - Column 1: entity type name
	 *
	 * @param[in] index Model index
	 * @param[in] role  Qt::ItemDataRole (Qt::DisplayRole, Qt::EditRole, Qt::DecorationRole, etc.)
	 * @return Data for the requested role, or invalid QVariant
	 */
	QVariant data(const QModelIndex& index, int role) const override;

	/**
	 * @brief Get the model index for a row/column/parent
	 * @param[in] row         Row index
	 * @param[in] column       Column index
	 * @param[in] parentIndex  Parent model index
	 * @return Model index for the cell, or invalid if out of bounds
	 */
	QModelIndex index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override;

	/**
	 * @brief Get the model index for a ccHObject
	 * @param[in] object Entity to get index for
	 * @return Model index, or invalid if object is not in the model
	 */
	QModelIndex index(ccHObject* object);

	/**
	 * @brief Get the parent model index
	 * @param[in] index Child model index
	 * @return Parent model index, or invalid for root-level items
	 */
	QModelIndex parent(const QModelIndex& index) const override;

	/**
	 * @brief Number of rows under a parent
	 * @param[in] parent Parent model index
	 * @return Number of child rows
	 */
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	/**
	 * @brief Number of columns (fixed at 2: name + type)
	 * @param[in] parent Parent model index
	 * @return Always 2
	 */
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/**
	 * @brief Get item flags for an index
	 *
	 * Returns Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled
	 * | Qt::ItemIsDropEnabled for column 0; Qt::NoItemFlags for column 1.
	 *
	 * @param[in] index Model index
	 * @return Item flags
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/**
	 * @brief Set data for an index (inline renaming)
	 *
	 * Supports Qt::EditRole to rename an entity in-place in the tree view.
	 *
	 * @param[in] index Model index
	 * @param[in] value New value (entity name for EditRole)
	 * @param[in] role  Qt::ItemDataRole
	 * @return true if the data was set successfully
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	/**
	 * @brief Supported drop actions (move only)
	 * @return Qt::MoveAction
	 */
	Qt::DropActions supportedDropActions() const override;

	/**
	 * @brief Handle a drop event
	 *
	 * Re-parents the dropped entity under the target node.
	 *
	 * @param[in] data     Dropped MIME data
	 * @param[in] action   Drop action type
	 * @param[in] row      Target row position
	 * @param[in] column   Target column
	 * @param[in] parent   Parent model index at drop position
	 * @return true if the drop was handled
	 */
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

	/**
	 * @brief Get all item data for an index (serialization)
	 * @param[in] index Model index
	 * @return Map of role → data
	 */
	QMap<int, QVariant> itemData(const QModelIndex& index) const override;

	/**
	 * @brief Supported drag actions
	 * @return Qt::MoveAction
	 */
	Qt::DropActions supportedDragActions() const override
	{
		return Qt::MoveAction;
	}

	/**
	 * @brief Handle selection change events
	 *
	 * Called by ccCustomQTreeView when the user changes the selection
	 * (click, Ctrl+click, Shift+click, etc.). Updates the properties
	 * panel and emits selectionChanged().
	 *
	 * @param[in] selected    Newly selected indices
	 * @param[in] deselected Newly deselected indices
	 */
	void changeSelection(const QItemSelection& selected, const QItemSelection& deselected);

	/**
	 * @brief Notify views that an entity's properties changed
	 *
	 * Called when an entity's name, color, visibility, or other
	 * displayable property changes. Forces a dataChanged() emit.
	 *
	 * @param[in] obj Entity whose properties changed
	 */
	void reflectObjectPropChange(ccHObject* obj);

	/**
	 * @brief Request redraw of a single entity's GL display
	 * @param[in] object Entity to redraw
	 */
	void redrawCCObject(ccHObject* object);

	/**
	 * @brief Request redraw of an entity and all its children
	 * @param[in] object Root entity to redraw
	 */
	void redrawCCObjectAndChildren(ccHObject* object);

	/**
	 * @brief Request full update of an entity in the DB tree
	 *
	 * Used after structural changes (children added/removed) to
	 * ensure the tree correctly reflects the hierarchy.
	 *
	 * @param[in] object Entity to update
	 */
	void updateCCObject(ccHObject* object);

	/**
	 * @brief Delete all selected entities from the DB and GL views
	 */
	void deleteSelectedEntities();

	/**
	 * @brief Select an entity
	 *
	 * Adds the entity to the current selection. If forceAdditiveSelection
	 * is false and Ctrl is not held, replaces the current selection.
	 *
	 * @param[in] obj                    Entity to select
	 * @param[in] forceAdditiveSelection If true, add to selection (Ctrl held)
	 */
	void selectEntity(ccHObject* obj, bool forceAdditiveSelection = false);

	/**
	 * @brief Select multiple entities by ID
	 * @param[in] entIDs Set of unique IDs to select
	 */
	void selectEntities(std::unordered_set<int> entIDs);

	/**
	 * @brief Select multiple entities by pointer
	 *
	 * @param[in] entities   Entities to select
	 * @param[in] incremental If true, add to current selection; if false, replace
	 */
	void selectEntities(const ccHObject::Container& entities, bool incremental = false);

  private:
	/**
	 * @brief Properties that can be toggled via context menu
	 */
	enum TOGGLE_PROPERTY
	{
		TG_ENABLE,   //!< Enable/disable entity
		TG_VISIBLE,  //!< Show/hide entity
		TG_COLOR,    //!< Show/hide entity color
		TG_SF,       //!< Show/hide scalar field
		TG_NORMAL,   //!< Show/hide normals
		TG_MATERIAL, //!< Show/hide materials/textures
		TG_3D_NAME   //!< Show/hide 3D name labels
	};

	/**
	 * @brief Apply a toggle property to all selected entities
	 * @param[in] prop Property to toggle
	 */
	void toggleSelectedEntitiesProperty(TOGGLE_PROPERTY prop);

	/**
	 * @brief Display the context menu at the current position
	 */
	void showContextMenu(const QPoint&);

	/**
	 * @brief Expand all branches of the tree
	 */
	void expandBranches();

	/**
	 * @brief Collapse all branches of the tree
	 */
	void collapseBranches();

	/**
	 * @brief Display information dialog for selected entities
	 */
	void gatherRecursiveInformation();

	/**
	 * @brief Sort children alphabetically (A → Z)
	 */
	void sortChildrenAZ();

	/**
	 * @brief Sort children reverse alphabetically (Z → A)
	 */
	void sortChildrenZA();

	/**
	 * @brief Sort children by entity type
	 */
	void sortChildrenType();

	/**
	 * @brief Open dialog to select entities by type and/or name
	 */
	void selectByTypeAndName();

	/**
	 * @brief Export visible images to files
	 */
	void exportImages();

	inline void toggleSelectedEntities()
	{
		toggleSelectedEntitiesProperty(TG_ENABLE);
	}
	inline void toggleSelectedEntitiesVisibility()
	{
		toggleSelectedEntitiesProperty(TG_VISIBLE);
	}
	inline void toggleSelectedEntitiesColor()
	{
		toggleSelectedEntitiesProperty(TG_COLOR);
	}
	inline void toggleSelectedEntitiesNormals()
	{
		toggleSelectedEntitiesProperty(TG_NORMAL);
	}
	inline void toggleSelectedEntitiesSF()
	{
		toggleSelectedEntitiesProperty(TG_SF);
	}
	inline void toggleSelectedEntitiesMat()
	{
		toggleSelectedEntitiesProperty(TG_MATERIAL);
	}
	inline void toggleSelectedEntities3DName()
	{
		toggleSelectedEntitiesProperty(TG_3D_NAME);
	}

	/**
	 * @brief Add an empty group entity
	 */
	void addEmptyGroup();

	/**
	 * @brief Align camera with entity using entity normals
	 */
	void alignCameraWithEntityDirect()
	{
		alignCameraWithEntity(false);
	}

	/**
	 * @brief Align camera with entity using inverted normals
	 */
	void alignCameraWithEntityIndirect()
	{
		alignCameraWithEntity(true);
	}

	/**
	 * @brief Enable bubble-view mode for a sensor entity
	 */
	void enableBubbleViewMode();

	/**
	 * @brief Edit the scalar value of a label entity
	 */
	void editLabelScalarValue();

  signals:
	/**
	 * @brief Emitted when the selection set changes
	 */
	void selectionChanged();

	/**
	 * @brief Emitted when the last entity is removed
	 */
	void dbIsEmpty();

	/**
	 * @brief Emitted when the first entity is added
	 */
	void dbIsNotEmptyAnymore();

  protected:
	/**
	 * @brief Align the 3D camera with an entity's orientation
	 *
	 * Uses the entity's plane normal (or polyline direction) to set
	 * the camera's view direction. Called from context menu actions.
	 *
	 * @param[in] reverse If true, use the inverse normal direction
	 */
	void alignCameraWithEntity(bool reverse);

	/**
	 * @brief Update the properties panel for an entity
	 *
	 * Rebuilds the QStandardItemModel in the properties panel with
	 * the editable properties of the given entity.
	 *
	 * @param[in] obj Entity to show properties for (nullptr hides the panel)
	 */
	void showPropertiesView(ccHObject* obj);

	/**
	 * @brief Sorting criteria for entity children
	 */
	enum SortRules
	{
		SORT_A2Z,    //!< Alphabetical ascending
		SORT_Z2A,    //!< Alphabetical descending
		SORT_BY_TYPE //!< Grouped by entity type
	};

	/**
	 * @brief Sort children of selected entities
	 * @param[in] rule Sorting rule to apply
	 */
	void sortSelectedEntitiesChildren(SortRules rule);

	/**
	 * @brief Expand or collapse selected tree items
	 * @param[in] expand true = expand, false = collapse
	 */
	void expandOrCollapseSelectedItems(bool expand);

	/**
	 * @brief Select entities matching type and/or name criteria
	 *
	 * @param[in] type            Entity type to match (e.g., CC_TYPES::MESH)
	 * @param[in] typeIsExclusive  If true, only exact type match; if false, include subclasses
	 * @param[in] name            Name pattern (exact or regex if nameIsRegex)
	 * @param[in] nameIsRegex     If true, match name as a regex
	 */
	void selectChildrenByTypeAndName(CC_CLASS_ENUM type,
	                                 bool typeIsExclusive = true,
	                                 QString name = QString(),
	                                 bool nameIsRegex = false);

	//! Underlying scene graph root
	ccHObject* m_treeRoot;

	//! DB tree view widget
	QTreeView* m_dbTreeWidget;

	//! Properties panel view widget
	QTreeView* m_propertiesTreeWidget;

	//! Data model for the properties panel
	QStandardItemModel* m_propertiesModel;

	//! Delegate for custom property editing
	ccPropertiesTreeDelegate* m_ccPropDelegate;

	// Context menu actions (one per menu item)
	QAction* m_expandSelectedItems;
	QAction* m_collapseSelectedItems;
	QAction* m_gatherInformation;
	QAction* m_sortChildrenAZ;
	QAction* m_sortChildrenZA;
	QAction* m_sortChildrenType;
	QAction* m_selectByTypeAndName;
	QAction* m_exportImages;
	QAction* m_deleteSelectedEntities;
	QAction* m_toggleSelectedEntities;
	QAction* m_toggleSelectedEntitiesVisibility;
	QAction* m_toggleSelectedEntitiesColor;
	QAction* m_toggleSelectedEntitiesNormals;
	QAction* m_toggleSelectedEntitiesMat;
	QAction* m_toggleSelectedEntitiesSF;
	QAction* m_toggleSelectedEntities3DName;
	QAction* m_addEmptyGroup;
	QAction* m_alignCameraWithEntity;
	QAction* m_alignCameraWithEntityReverse;
	QAction* m_enableBubbleViewMode;
	QAction* m_editLabelScalarValue;

	//! Position of the last right-click (for context menu placement)
	QPoint m_contextMenuPos;
};

#endif
