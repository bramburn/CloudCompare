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

// Local
#include "ccMainAppInterface.h"
#include "ccOverlayDialog.h"
#include "ccPickingListener.h"

// CCCoreLib
#include <PointProjectionTools.h>

// qCC_db
#include <ccPointCloud.h>

/**
 * @file ccPointPairRegistrationDlg.h
 *
 * @brief Point pair registration dialog using Horn's algorithm.
 *
 * @details Dialog for interactive point-pair based registration
 * (coarse alignment) of point clouds.
 *
 * Uses Horn's absolute orientation algorithm (1987) to compute
 * the optimal rotation and translation between two point sets
 * using corresponding point pairs.
 *
 * Features:
 * - Interactive point picking on both clouds
 * - Automatic sphere center detection for precise picking
 * - RMS error reporting
 * - Manual point entry
 * - Point management (add, remove, clear)
 *
 * The user picks corresponding points on both the "aligned"
 * (to-be-registered) and "reference" clouds.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see CCCoreLib::HornRegistrationTools
 */

// Qt generated dialog
#include <ui_pointPairRegistrationDlg.h>

class ccGenericPointCloud;
class ccGenericGLDisplay;
class ccGLWindowInterface;
class cc2DLabel;
class ccPickingHub;

/**
 * @brief Dialog for point-pair based registration.
 *
 * @details Provides an interactive interface for coarse point cloud
 * registration using corresponding point pairs.
 *
 * The registration uses Horn's absolute orientation algorithm
 * to compute the rigid transformation (rotation + translation)
 * that best aligns the two point sets.
 *
 * Usage:
 * 1. Pick points on the aligned cloud
 * 2. Pick corresponding points on the reference cloud
 * 3. Click "Align" to compute the transformation
 * 4. Apply to register the clouds
 *
 * @extends ccOverlayDialog
 * @extends ccPickingListener
 * @extends Ui::pointPairRegistrationDlg
 */
class ccPointPairRegistrationDlg : public ccOverlayDialog
    , public ccPickingListener
    , Ui::pointPairRegistrationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the registration dialog.
	 *
	 * @param[in] pickingHub Picking hub for point selection.
	 * @param[in] app Main application interface.
	 * @param[in] parent Parent widget.
	 */
	explicit ccPointPairRegistrationDlg(ccPickingHub* pickingHub, ccMainAppInterface* app, QWidget* parent = nullptr);

	// inherited from ccOverlayDialog
	/**
	 * @brief Link with a 3D window.
	 * @param[in] win Window to link with.
	 * @return true on success.
	 */
	bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the dialog.
	 * @return true on success.
	 */
	bool start() override;

	/**
	 * @brief Stop the dialog.
	 * @param[in] state Final state.
	 */
	void stop(bool state) override;

	/**
	 * @brief Initialize with entities.
	 *
	 * @param[in] win 3D window.
	 * @param[in] alignedEntities Entities to be aligned.
	 * @param[in] referenceEntities Reference entities (optional).
	 * @return true on success.
	 */
	bool init(ccGLWindowInterface*        win,
	          const ccHObject::Container& alignedEntities,
	          const ccHObject::Container* referenceEntities = nullptr);

	/**
	 * @brief Clear all picked points.
	 */
	void clear();

	/**
	 * @brief Pause/resume the dialog.
	 * @param[in] state Pause state.
	 */
	void pause(bool state);

	/**
	 * @brief Add a point to the aligned set.
	 *
	 * @param[in,out] P Point coordinates.
	 * @param[in] entity Associated entity.
	 * @param[in] shifted Whether point is shifted.
	 * @return true on success.
	 */
	bool addAlignedPoint(CCVector3d& P, ccHObject* entity = nullptr, bool shifted = true);

	/**
	 * @brief Add a point to the reference set.
	 *
	 * @param[in,out] P Point coordinates.
	 * @param[in] entity Associated entity.
	 * @param[in] shifted Whether point is shifted.
	 * @return true on success.
	 */
	bool addReferencePoint(CCVector3d& P, ccHObject* entity = nullptr, bool shifted = true);

	/**
	 * @brief Remove a point from the aligned set.
	 *
	 * @param[in] index Point index.
	 * @param[in] autoRemoveDualPoint Remove corresponding ref point.
	 */
	void removeAlignedPoint(int index, bool autoRemoveDualPoint = true);

	/**
	 * @brief Remove a point from the reference set.
	 *
	 * @param[in] index Point index.
	 * @param[in] autoRemoveDualPoint Remove corresponding aligned point.
	 */
	void removeRefPoint(int index, bool autoRemoveDualPoint = true);

	// inherited from ccPickingListener
	/**
	 * @brief Handle picked item.
	 *
	 * @param[in] pi Picked item information.
	 */
	void onItemPicked(const PickedItem& pi) override;

  protected slots:
	/**
	 * @brief Toggle aligned entities visibility.
	 * @param[in] state Visibility state.
	 */
	void showAlignedEntities(bool state);

	/**
	 * @brief Toggle reference entities visibility.
	 * @param[in] state Visibility state.
	 */
	void showReferenceEntities(bool state);

	/**
	 * @brief Add manual point to aligned set.
	 */
	void addManualAlignedPoint();

	/**
	 * @brief Add manual point to reference set.
	 */
	void addManualRefPoint();

	/**
	 * @brief Remove last aligned point.
	 */
	void unstackAligned();

	/**
	 * @brief Remove last reference point.
	 */
	void unstackRef();

	/**
	 * @brief Handle delete button.
	 */
	void onDelButtonPushed();

	/**
	 * @brief Update registration info.
	 */
	void updateAlignInfo();

	/**
	 * @brief Apply the transformation.
	 */
	void apply();

	/**
	 * @brief Perform alignment.
	 */
	void align();

	/**
	 * @brief Reset the dialog.
	 */
	void reset();

	/**
	 * @brief Cancel and close.
	 */
	void cancel();

  protected:
	/**
	 * @brief Handle point count change.
	 */
	void onPointCountChanged();

	/**
	 * @brief Call the registration algorithm.
	 *
	 * @param[out] trans Transformation result.
	 * @param[out] rms RMS error.
	 * @param[in] autoUpdateTab Auto-update table.
	 * @param[out] withUmeyama Whether Umeyama was used.
	 * @param[out] report Optional report strings.
	 * @return true on success.
	 */
	bool callRegistration(CCCoreLib::PointProjectionTools::Transformation& trans,
	                      double&                                          rms,
	                      bool                                             autoUpdateTab,
	                      bool&                                            withUmeyama,
	                      QStringList*                                     report = nullptr);

	/**
	 * @brief Clear RMS columns.
	 */
	void clearRMSColumns();

	/**
	 * @brief Add point to table widget.
	 *
	 * @param[in] tableWidget Table to add to.
	 * @param[in] rowIndex Row index.
	 * @param[in] P Point coordinates.
	 * @param[in] pointLabel Label text.
	 */
	void addPointToTable(QTableWidget*     tableWidget,
	                     int               rowIndex,
	                     const CCVector3d& P,
	                     QString           pointLabel);

	/**
	 * @brief Convert picked point to sphere center.
	 *
	 * @param[in,out] P Point coordinates.
	 * @param[in] entity Associated entity.
	 * @param[out] sphereRadius Detected sphere radius.
	 * @return true if point can be used.
	 */
	bool convertToSphereCenter(CCVector3d& P, ccHObject* entity, PointCoordinateType& sphereRadius);

	/**
	 * @brief Reset the displayed title.
	 */
	void resetTitle();

	/**
	 * @brief Entity original context for restoration.
	 */
	struct EntityContext
	{
		/**
		 * @brief Construct entity context.
		 * @param[in] ent Entity.
		 */
		explicit EntityContext(ccHObject* ent);

		/**
		 * @brief Restore original state.
		 */
		void restore();

		ccHObject*          entity;
		ccGenericGLDisplay* originalDisplay;
		bool                wasVisible;
		bool                wasEnabled;
		bool                wasSelected;
	};

	/**
	 * @brief Set of entity contexts.
	 */
	struct EntityContexts : public QMap<ccHObject*, EntityContext>
	{
		/**
		 * @brief Fill from entities.
		 * @param[in] entities Entities to store.
		 */
		void fill(const ccHObject::Container& entities);

		/**
		 * @brief Restore all entities.
		 */
		void restoreAll()
		{
			for (EntityContext& ctx : *this)
				ctx.restore();
		}

		bool       isShifted;
		CCVector3d shift;
		double     scale = 1.0;
	};

	/**
	 * @brief Remove a label.
	 *
	 * @param[in,out] points Point cloud.
	 * @param[in] childIndex Child index.
	 * @param[in] entities Entity contexts.
	 */
	void removeLabel(ccPointCloud&         points,
	                 unsigned              childIndex,
	                 const EntityContexts& entities);

  protected: // members
	//! To-be-aligned entities
	EntityContexts m_alignedEntities;

	//! To-be-aligned points set
	ccPointCloud m_alignedPoints;

	//! Reference entities
	EntityContexts m_referenceEntities;

	//! Reference points set
	ccPointCloud m_refPoints;

	//! Whether paused
	bool m_paused;

	//! Picking hub
	ccPickingHub* m_pickingHub;

	//! Main application interface
	ccMainAppInterface* m_app;
};
