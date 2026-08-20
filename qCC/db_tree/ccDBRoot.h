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

#ifndef CC_DB_ROOT_HEADER
#define CC_DB_ROOT_HEADER

// Qt
#include <QAbstractItemModel>
#include <QPoint>
#include <QTreeView>

// qCC_db
#include <ccHObject.h>

/**
 * @file ccDBRoot.h
 *
 * @brief Database tree root and management for CloudCompare.
 *
 * @details This module provides the database tree management functionality
 * for CloudCompare. The database tree displays all loaded entities
 * (point clouds, meshes, primitives, etc.) in a hierarchical structure.
 *
 * Key responsibilities:
 * - Maintains the hierarchical entity tree (ccHObject)
 * - Provides QAbstractItemModel interface for QTreeView
 * - Handles selection management across the tree
 * - Manages the properties panel for selected entities
 * - Provides context menu actions for entity manipulation
 *
 * @section architecture Architecture
 *
 * The database tree is implemented as a Qt Model/View architecture:
 * - ccDBRoot: QAbstractItemModel (the model)
 * - ccCustomQTreeView: QTreeView (the view)
 * - ccPropertiesTreeDelegate: displays entity properties
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccHObject
 * @see MainWindow
 */

// System
#include <unordered_set>

class QAction;
class QStandardItemModel;

class ccPropertiesTreeDelegate;
class ccHObject;

/**
 * @brief Statistics about the current selection in the database tree.
 *
 * @details Provides counts of different entity types and components
 * within the current selection. Used to enable/disable menu actions
 * based on what is selected.
 */
struct dbTreeSelectionInfo
{
	size_t selCount          = 0;  //!< Total selected entities count
	size_t sfCount           = 0;  //!< Selected entities with scalar fields
	size_t colorCount        = 0;  //!< Selected entities with colors
	size_t normalsCount      = 0;  //!< Selected entities with normals
	size_t octreeCount       = 0;  //!< Selected entities with octrees
	size_t cloudCount        = 0;  //!< Selected point clouds
	size_t gridCound         = 0;  //!< Selected 2D grids
	size_t groupCount        = 0;  //!< Selected groups
	size_t polylineCount     = 0;  //!< Selected polylines
	size_t planeCount        = 0;  //!< Selected planes
	size_t circleCount       = 0;  //!< Selected circles
	size_t meshCount         = 0;  //!< Selected meshes
	size_t primitiveCount    = 0;  //!< Selected primitives
	size_t imageCount        = 0;  //!< Selected images
	size_t sensorCount       = 0;  //!< Selected sensors
	size_t gblSensorCount    = 0;  //!< Selected GBLS sensors
	size_t cameraSensorCount = 0;  //!< Selected camera sensors
	size_t kdTreeCount       = 0;  //!< Selected KD-trees
};

/**
 * @brief Custom QTreeView with advanced selection behavior.
 *
 * @details Extends QTreeView to customize how selections are handled
 * when clicking on items. Currently provides specific behavior for
 * handling multi-selection with modifier keys.
 *
 * @extends QTreeView
 */
class ccCustomQTreeView : public QTreeView
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the custom tree view.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccCustomQTreeView(QWidget* parent)
	    : QTreeView(parent)
	{
	}

  protected:
	/**
	 * @brief Determine selection behavior for an index.
	 *
	 * @param[in] index Model index.
	 * @param[in] event Associated event (if any).
	 * @return Selection flags.
	 */
	QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index, const QEvent* event = nullptr) const override;
};

/**
 * @brief Database tree root for CloudCompare.
 *
 * @details The ccDBRoot class is the root of the entity hierarchy in
 * CloudCompare. It serves as a QAbstractItemModel for the Qt tree view
 * that displays all loaded entities.
 *
 * The class manages:
 * - The root ccHObject that contains all entities
 * - Selection state and selection-based operations
 * - Properties panel for displaying selected entity details
 * - Context menus for entity manipulation
 *
 * @extends QAbstractItemModel
 *
 * @par Usage
 * @code
 * ccDBRoot* dbRoot = new ccDBRoot(ui.dbTreeView, ui.propertiesView, this);
 * dbRoot->addElement(newEntity);
 * @endcode
 *
 * @see ccHObject
 * @see ccCustomQTreeView
 * @see ccPropertiesTreeDelegate
 */
class ccDBRoot : public QAbstractItemModel
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the database tree root.
	 *
	 * @param[in] dbTreeWidget Widget for displaying the database tree.
	 * @param[in] propertiesTreeWidget Widget for displaying entity properties.
	 * @param[in] parent Parent QObject.
	 */
	ccDBRoot(ccCustomQTreeView* dbTreeWidget, QTreeView* propertiesTreeWidget, QObject* parent = nullptr);

	/**
	 * @brief Destructor.
	 *
	 * @details Cleans up the tree root and all child entities.
	 */
	~ccDBRoot() override;

	/**
	 * @brief Get the root entity of the database tree.
	 *
	 * @return Pointer to the root ccHObject.
	 */
	ccHObject* getRootEntity();

	/**
	 * @brief Hide the properties view panel.
	 */
	void hidePropertiesView();

	/**
	 * @brief Update the properties view for the current selection.
	 */
	void updatePropertiesView();

	/**
	 * @brief Add an entity to the database tree.
	 *
	 * @param[in] object Entity to add.
	 * @param[in] autoExpand Whether to automatically expand the tree to show the new entity.
	 */
	void addElement(ccHObject* object, bool autoExpand = true);

	/**
	 * @brief Remove an entity from the database tree.
	 *
	 * @param[in] object Entity to remove.
	 *
	 * @note Calls prepareDisplayForRefresh on the object before removal.
	 */
	void removeElement(ccHObject* object);

	/**
	 * @brief Remove multiple entities from the database tree.
	 *
	 * @param[in,out] objects Container of entities to remove.
	 *
	 * @note Faster than calling removeElement() multiple times.
	 * @note The input container is cleared after this operation.
	 */
	void removeElements(ccHObject::Container& objects);

	/**
	 * @brief Find an entity by its unique ID.
	 *
	 * @param[in] uniqueID Unique identifier to search for.
	 * @return Pointer to the entity, or nullptr if not found.
	 */
	ccHObject* find(int uniqueID) const;

	/**
	 * @brief Count selected entities of a specific type.
	 *
	 * @param[in] filter Entity type filter (default: all types).
	 * @return Number of selected entities matching the filter.
	 */
	int countSelectedEntities(CC_CLASS_ENUM filter = CC_TYPES::OBJECT);

	/**
	 * @brief Get all selected entities.
	 *
	 * @param[out] selectedEntities Container to store selected entities.
	 * @param[in] filter Entity type filter (default: all types).
	 * @param[out] info Optional pointer to receive selection statistics.
	 * @return Number of selected entities.
	 */
	size_t getSelectedEntities(ccHObject::Container& selectedEntities,
	                           CC_CLASS_ENUM         filter = CC_TYPES::OBJECT,
	                           dbTreeSelectionInfo*  info   = nullptr);

	/**
	 * @brief Expand or collapse an entity in the tree.
	 *
	 * @param[in] object Entity to expand/collapse.
	 * @param[in] state true to expand, false to collapse.
	 */
	void expandElement(ccHObject* object, bool state);

	/**
	 * @brief Unselect a specific entity.
	 *
	 * @param[in] obj Entity to unselect.
	 */
	void unselectEntity(ccHObject* obj);

	/**
	 * @brief Unselect all entities.
	 */
	void unselectAllEntities();

	/**
	 * @brief Unload all entities from the database.
	 */
	void unloadAll();

	// inherited from QAbstractItemModel
	QVariant            data(const QModelIndex& index, int role) const override;
	QModelIndex         index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override;
	QModelIndex         index(ccHObject* object);
	QModelIndex         parent(const QModelIndex& index) const override;
	int                 rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int                 columnCount(const QModelIndex& parent = QModelIndex()) const override;
	Qt::ItemFlags       flags(const QModelIndex& index) const override;
	bool                setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::DropActions     supportedDropActions() const override;
	bool                dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
	QMap<int, QVariant> itemData(const QModelIndex& index) const override;
	Qt::DropActions     supportedDragActions() const override
	{
		return Qt::MoveAction;
	}

	/**
	 * @brief Change selection state based on selection deltas.
	 *
	 * @param[in] selected Items that became selected.
	 * @param[in] deselected Items that became deselected.
	 */
	void changeSelection(const QItemSelection& selected, const QItemSelection& deselected);

	/**
	 * @brief Reflect a property change on an entity.
	 *
	 * @param[in] obj Entity whose property changed.
	 */
	void reflectObjectPropChange(ccHObject* obj);

	/**
	 * @brief Redraw a specific entity in the 3D view.
	 *
	 * @param[in] object Entity to redraw.
	 */
	void redrawCCObject(ccHObject* object);

	/**
	 * @brief Redraw an entity and all its children.
	 *
	 * @param[in] object Root entity to redraw.
	 */
	void redrawCCObjectAndChildren(ccHObject* object);

	/**
	 * @brief Update an entity in the tree.
	 *
	 * @param[in] object Entity to update.
	 */
	void updateCCObject(ccHObject* object);

	/**
	 * @brief Delete all selected entities.
	 */
	void deleteSelectedEntities();

	/**
	 * @brief Select a specific entity.
	 *
	 * @param[in] obj Entity to select.
	 * @param[in] forceAdditiveSelection If true, add to existing selection.
	 *
	 * @note If Ctrl is pressed, behaves as additive selection.
	 */
	void selectEntity(ccHObject* obj, bool forceAdditiveSelection = false);

	/**
	 * @brief Select multiple entities by their unique IDs.
	 *
	 * @param[in] entIDs Set of unique IDs to select.
	 */
	void selectEntities(std::unordered_set<int> entIDs);

	/**
	 * @brief Select multiple entities.
	 *
	 * @param[in] entities Set of entities to select.
	 * @param[in] incremental If true, add to existing selection.
	 */
	void selectEntities(const ccHObject::Container& entities, bool incremental = false);

  signals:
	/**
	 * @brief Emitted when the selection changes.
	 */
	void selectionChanged();

	/**
	 * @brief Emitted when the database becomes empty.
	 */
	void dbIsEmpty();

	/**
	 * @brief Emitted when the database becomes non-empty.
	 */
	void dbIsNotEmptyAnymore();

  protected:
	/**
	 * @brief Align the 3D view camera with an entity.
	 *
	 * @param[in] reverse If true, use the entity's inverse normal.
	 */
	void alignCameraWithEntity(bool reverse);

	/**
	 * @brief Show properties for an entity.
	 *
	 * @param[in] obj Entity to show properties for.
	 */
	void showPropertiesView(ccHObject* obj);

	/**
	 * @brief Sorting rules for entities.
	 */
	enum SortRules
	{
		SORT_A2Z,      //!< Sort alphabetically A to Z
		SORT_Z2A,      //!< Sort alphabetically Z to A
		SORT_BY_TYPE   //!< Sort by entity type
	};

	/**
	 * @brief Sort children of selected entities.
	 *
	 * @param[in] rule Sorting rule to apply.
	 */
	void sortSelectedEntitiesChildren(SortRules rule);

	/**
	 * @brief Expand or collapse selected items.
	 *
	 * @param[in] expand true to expand, false to collapse.
	 */
	void expandOrCollapseSelectedItems(bool expand);

	/**
	 * @brief Select children by type and/or name.
	 *
	 * @param[in] type Entity type to select.
	 * @param[in] typeIsExclusive If true, only select entities of this exact type.
	 * @param[in] name Name filter (can be regex).
	 * @param[in] nameIsRegex If true, treat name as regex pattern.
	 */
	void selectChildrenByTypeAndName(CC_CLASS_ENUM type,
	                                 bool          typeIsExclusive = true,
	                                 QString       name            = QString(),
	                                 bool          nameIsRegex     = false);

	//! Entity property toggle types
	enum TOGGLE_PROPERTY
	{
		TG_ENABLE,      //!< Enable/disable entity
		TG_VISIBLE,     //!< Show/hide entity
		TG_COLOR,       //!< Show/hide color
		TG_SF,          //!< Show/hide scalar field
		TG_NORMAL,      //!< Show/hide normals
		TG_MATERIAL,    //!< Show/hide materials
		TG_3D_NAME      //!< Show/hide 3D name
	};

	/**
	 * @brief Toggle a property on selected entities.
	 *
	 * @param[in] prop Property to toggle.
	 */
	void toggleSelectedEntitiesProperty(TOGGLE_PROPERTY prop);

	//! Root ccHObject containing all entities
	ccHObject* m_treeRoot;

	//! Widget for displaying the database tree
	QTreeView* m_dbTreeWidget;

	//! Widget for displaying entity properties
	QTreeView* m_propertiesTreeWidget;

	//! Model for properties tree
	QStandardItemModel* m_propertiesModel;

	//! Delegate for properties tree
	ccPropertiesTreeDelegate* m_ccPropDelegate;

	//! Context menu actions
	QAction* m_expandSelectedItems;              //!< Expand selected items
	QAction* m_collapseSelectedItems;             //!< Collapse selected items
	QAction* m_gatherInformation;                //!< Gather entity information
	QAction* m_sortChildrenAZ;                    //!< Sort A to Z
	QAction* m_sortChildrenZA;                   //!< Sort Z to A
	QAction* m_sortChildrenType;                  //!< Sort by type
	QAction* m_selectByTypeAndName;              //!< Select by type/name
	QAction* m_exportImages;                      //!< Export images
	QAction* m_deleteSelectedEntities;            //!< Delete selected
	QAction* m_toggleSelectedEntities;            //!< Enable/disable
	QAction* m_toggleSelectedEntitiesVisibility;  //!< Show/hide
	QAction* m_toggleSelectedEntitiesColor;      //!< Show/hide color
	QAction* m_toggleSelectedEntitiesNormals;     //!< Show/hide normals
	QAction* m_toggleSelectedEntitiesMat;         //!< Show/hide materials
	QAction* m_toggleSelectedEntitiesSF;          //!< Show/hide scalar field
	QAction* m_toggleSelectedEntities3DName;      //!< Show/hide 3D name
	QAction* m_addEmptyGroup;                     //!< Add empty group
	QAction* m_alignCameraWithEntity;             //!< Align camera
	QAction* m_alignCameraWithEntityReverse;      //!< Align camera (reverse)
	QAction* m_enableBubbleViewMode;              //!< Enable bubble view
	QAction* m_editLabelScalarValue;              //!< Edit scalar value

	//! Last context menu position
	QPoint m_contextMenuPos;

  private:
	/**
	 * @brief Show the context menu at current position.
	 */
	void showContextMenu(const QPoint&);

	/**
	 * @brief Expand all branches.
	 */
	void expandBranches();

	/**
	 * @brief Collapse all branches.
	 */
	void collapseBranches();

	/**
	 * @brief Gather recursive information on selected entities.
	 */
	void gatherRecursiveInformation();

	/**
	 * @brief Sort children alphabetically (A to Z).
	 */
	void sortChildrenAZ();

	/**
	 * @brief Sort children alphabetically (Z to A).
	 */
	void sortChildrenZA();

	/**
	 * @brief Sort children by type.
	 */
	void sortChildrenType();

	/**
	 * @brief Show dialog to select by type and name.
	 */
	void selectByTypeAndName();

	/**
	 * @brief Export selected images.
	 */
	void exportImages();

	/**
	 * @brief Add an empty group to the database.
	 */
	void addEmptyGroup();

	/**
	 * @brief Align camera with entity (direct).
	 */
	void alignCameraWithEntityDirect()
	{
		alignCameraWithEntity(false);
	}

	/**
	 * @brief Align camera with entity (indirect).
	 */
	void alignCameraWithEntityIndirect()
	{
		alignCameraWithEntity(true);
	}

	/**
	 * @brief Enable bubble view mode.
	 */
	void enableBubbleViewMode();

	/**
	 * @brief Edit scalar value via 2D label.
	 */
	void editLabelScalarValue();

	//! Inline toggles
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
};

#endif
