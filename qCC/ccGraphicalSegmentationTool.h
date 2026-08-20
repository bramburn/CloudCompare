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
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccGraphicalSegmentationTool.h
 *
 * @brief Interactive segmentation tool for point cloud extraction.
 *
 * @details Tool for segmenting point clouds by drawing selection regions
 * directly on the 3D view.
 *
 * Segmentation modes:
 * - **Polyline**: Draw freeform polygon to select points
 * - **Rectangle**: Draw rectangular selection
 *
 * Actions:
 * - **Segment in**: Keep points inside selection
 * - **Segment out**: Remove points inside selection
 *
 * Features:
 * - Multiple entity support
 * - Undo/redo via polyline editing
 * - Export segmentation polyline
 * - Classification value assignment
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccOverlayDialog
 * @see ccGraphicalSegmentationOptionsDlg
 */

#include <ccOverlayDialog.h>

// qCC_db
#include <ccHObject.h>

// Qt
#include <QSet>

// GUI
#include <set>
#include <ui_graphicalSegmentationDlg.h>

class ccPolyline;
class ccPointCloud;
class ccGLWindowInterface;
class ccMainAppInterface;

/**
 * @brief Interactive segmentation tool for point clouds.
 *
 * @details Provides an overlay dialog for graphical segmentation.
 *
 * Usage:
 * 1. Add entities to segment
 * 2. Choose polyline or rectangle mode
 * 3. Draw selection region in 3D view
 * 4. Segment in or segment out
 * 5. Apply or cancel
 *
 * @extends ccOverlayDialog
 */
class ccGraphicalSegmentationTool : public ccOverlayDialog
    , public Ui::GraphicalSegmentationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Process states.
	 */
	enum ProcessStates
	{
		POLYLINE  = 1, //!< Polyline selection mode
		RECTANGLE = 2, //!< Rectangle selection mode
		PAUSED    = 32, //!< Tool paused
		STARTED   = 64, //!< Tool started
		RUNNING   = 128 //!< Processing
	};

	/**
	 * @brief Construct the segmentation tool.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccGraphicalSegmentationTool(QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccGraphicalSegmentationTool();

	/**
	 * @brief Add an entity to the segmentation pool.
	 *
	 * @param[in] anObject Entity to add.
	 * @param[in] silent Suppress error messages.
	 *
	 * @return true if entity was added.
	 *
	 * @note Locked entities may be rejected.
	 */
	bool addEntity(ccHObject* anObject, bool silent = false);

	/**
	 * @brief Get number of valid entities.
	 *
	 * @return Count of entities in pool.
	 */
	unsigned getNumberOfValidEntities() const;

	/**
	 * @brief Get the segmentation polyline.
	 *
	 * @return Polyline defining the selection.
	 */
	const ccPolyline* getPolyLine() const
	{
		return m_segmentationPoly;
	}

	/**
	 * @brief Get entities to segment.
	 *
	 * @return Set of entities.
	 */
	QSet<ccHObject*>& entities()
	{
		return m_toSegment;
	}

	/**
	 * @brief Get entities to segment (const).
	 *
	 * @return Set of entities.
	 */
	const QSet<ccHObject*>& entities() const
	{
		return m_toSegment;
	}

	// inherited from ccOverlayDialog
	/**
	 * @brief Link with a 3D window.
	 */
	virtual bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the tool.
	 */
	virtual bool start() override;

	/**
	 * @brief Stop the tool.
	 *
	 * @param[in] accepted Whether to apply changes.
	 */
	virtual void stop(bool accepted) override;

	/**
	 * @brief Check if hidden parts should be deleted.
	 *
	 * @return true if deleting hidden parts.
	 */
	bool deleteHiddenParts() const
	{
		return m_deleteHiddenParts;
	}

	/**
	 * @brief Remove all entities from pool.
	 *
	 * @note Calls unallocateVisibilityArray on all clouds.
	 */
	void removeAllEntities();

	/**
	 * @brief Apply segmentation and update DB.
	 *
	 * @param[in] app Main application interface.
	 * @param[out] newEntities Created entities.
	 *
	 * @return true on success.
	 */
	bool applySegmentation(ccMainAppInterface* app, ccHObject::Container& newEntities);

  signals:
	/**
	 * @brief Emitted when current SF changes.
	 */
	void currentScalarFieldUpdated();

  protected slots:
	/**
	 * @brief Segment points inside selection.
	 */
	void segmentIn();

	/**
	 * @brief Segment points outside selection.
	 */
	void segmentOut();

	/**
	 * @brief Export current selection.
	 */
	void exportSelection();

	/**
	 * @brief Perform segmentation.
	 *
	 * @param[in] keepPointsInside Keep or remove inside.
	 * @param[in] classificationValue SF value for classification.
	 * @param[in] exportSelection Export instead of segment.
	 */
	void segment(bool keepPointsInside, ScalarType classificationValue = CCCoreLib::NAN_VALUE, bool exportSelection = false);

	/**
	 * @brief Reset segmentation.
	 */
	void reset();

	/**
	 * @brief Show options dialog.
	 */
	void options();

	/**
	 * @brief Apply and keep result.
	 */
	void apply();

	/**
	 * @brief Apply and delete hidden parts.
	 */
	void applyAndDelete();

	/**
	 * @brief Cancel segmentation.
	 */
	void cancel();

	/**
	 * @brief Add point to polyline.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 */
	void addPointToPolyline(int x, int y)
	{
		return addPointToPolylineExt(x, y, false);
	}

	/**
	 * @brief Add point to polyline (extended).
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 * @param[in] allowClicksOutside Allow outside viewport.
	 */
	void addPointToPolylineExt(int x, int y, bool allowClicksOutside);

	/**
	 * @brief Close the polyline.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 */
	void closePolyLine(int x = 0, int y = 0);

	/**
	 * @brief Close rectangle selection.
	 */
	void closeRectangle();

	/**
	 * @brief Update polyline preview.
	 *
	 * @param[in] x Screen X.
	 * @param[in] y Screen Y.
	 * @param[in] buttons Mouse buttons.
	 */
	void updatePolyLine(int x, int y, Qt::MouseButtons buttons);

	/**
	 * @brief Run segmentation.
	 */
	void run();

	/**
	 * @brief Stop running.
	 */
	void stopRunning();

	/**
	 * @brief Pause/resume segmentation.
	 *
	 * @param[in] state Pause state.
	 */
	void pauseSegmentationMode(bool state);

	/**
	 * @brief Set classification value.
	 */
	void setClassificationValue();

	/**
	 * @brief Set polyline selection mode.
	 */
	void doSetPolylineSelection();

	/**
	 * @brief Set rectangular selection mode.
	 */
	void doSetRectangularSelection();

	/**
	 * @brief Use existing polyline for selection.
	 */
	void doActionUseExistingPolyline();

	/**
	 * @brief Export segmentation polyline.
	 */
	void doExportSegmentationPolyline();

	/**
	 * @brief Toggle RGB and SF colors.
	 */
	void onToggleRGBAndSFColors();

	/**
	 * @brief Handle shortcut trigger.
	 *
	 * @param[in] id Shortcut ID.
	 */
	void onShortcutTriggered(int id);

  protected:
	/**
	 * @brief Prepare entity for removal.
	 *
	 * @param[in] entity Entity to remove.
	 * @param[in] unallocateVisibilityArrays Unallocate visibility arrays.
	 */
	void prepareEntityForRemoval(ccHObject* entity, bool unallocateVisibilityArrays);

	/**
	 * @brief Allow polyline export.
	 *
	 * @param[in] state Enable/disable export.
	 */
	void allowPolylineExport(bool state);

  private:
	//! Entities to segment
	QSet<ccHObject*> m_toSegment;

	//! Something changed flag
	bool m_somethingHasChanged;

	//! Process state
	unsigned m_state;

	//! Segmentation polyline
	ccPolyline* m_segmentationPoly;

	//! Polyline vertices
	ccPointCloud* m_polyVertices;

	//! Rectangular selection mode
	bool m_rectangularSelection;

	//! Delete hidden parts
	bool m_deleteHiddenParts;

	//! Entities to enable on close
	std::set<ccHObject*> m_enableOnClose;

	//! Entities to disable on close
	std::set<ccHObject*> m_disableOnClose;
};
