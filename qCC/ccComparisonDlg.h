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

#ifndef CC_COMPARISON_DIALOG_HEADER
#define CC_COMPARISON_DIALOG_HEADER

/**
 * @file ccComparisonDlg.h
 *
 * @brief Comparison dialog for cloud/cloud or cloud/mesh distance computation.
 *
 * @details Dialog for computing distances between two entities:
 * - Cloud-to-cloud (C2C): Point-to-point distances between two point clouds
 * - Cloud-to-mesh (C2M): Distances from cloud points to mesh surface
 *
 * The comparison outputs a scalar field on the compared cloud
 * containing the distance values, along with statistics
 * (min, max, mean, standard deviation, etc.).
 *
 * Uses octree-based spatial indexing for efficient nearest-neighbor queries.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEntityAction::computeCloudCloudDist()
 * @see ccEntityAction::computeCloudMeshDist()
 */

// qCC_db
#include <ccOctree.h>

// Qt
#include <QDialog>
#include <QString>
#include <ui_comparisonDlg.h>

class ccHObject;
class ccPointCloud;
class ccGenericPointCloud;
class ccGenericMesh;

/**
 * @brief Dialog for cloud/cloud or cloud/mesh comparison.
 *
 * @details Provides a UI for configuring and running distance
 * comparisons between two entities.
 *
 * Features:
 * - Cloud-to-cloud distance computation
 * - Cloud-to-mesh distance computation
 * - Octree-based spatial indexing for efficiency
 * - Configurable maximum search distance
 * - 2D mode (ignore Z differences)
 * - Histogram display of distance distribution
 * - Scalar field output with distance values
 *
 * The dialog:
 * 1. Loads both entities
 * 2. Builds octrees for efficient queries
 * 3. Computes approximate distances (for histogram preview)
 * 4. User adjusts parameters
 * 5. Full comparison is computed
 *
 * @extends QDialog
 * @extends Ui::ComparisonDialog
 */
class ccComparisonDlg : public QDialog
    , public Ui::ComparisonDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Comparison types.
	 */
	enum CC_COMPARISON_TYPE
	{
		CLOUDCLOUD_DIST = 0, //!< Cloud to cloud comparison
		CLOUDMESH_DIST  = 1, //!< Cloud to mesh comparison
	};

	/**
	 * @brief Construct the comparison dialog.
	 *
	 * @param[in] compEntity The entity to compare (will get distance SF).
	 * @param[in] refEntity The reference entity.
	 * @param[in] cpType Comparison type.
	 * @param[in] parent Parent widget.
	 * @param[in] noDisplay If true, suppress display updates.
	 */
	ccComparisonDlg(ccHObject*         compEntity,
	                ccHObject*         refEntity,
	                CC_COMPARISON_TYPE cpType,
	                QWidget*           parent    = nullptr,
	                bool               noDisplay = false);

	/**
	 * @brief Destructor.
	 */
	~ccComparisonDlg();

	/**
	 * @brief Initialize the dialog.
	 *
	 * @return true on success.
	 *
	 * @details Computes approximate distances for histogram
	 * preview and determines optimal octree level.
	 */
	inline bool initDialog()
	{
		return computeApproxDistances();
	}

	/**
	 * @brief Get the compared entity.
	 *
	 * @return The entity that was compared.
	 */
	ccHObject* getComparedEntity() const
	{
		return m_compEnt;
	}

	/**
	 * @brief Get the reference entity.
	 *
	 * @return The reference entity.
	 */
	ccHObject* getReferenceEntity()
	{
		return m_refEnt;
	}

  public slots:
	/**
	 * @brief Compute full distances.
	 * @return true on success.
	 */
	bool computeDistances();

	/**
	 * @brief Apply results and close dialog.
	 */
	void applyAndExit();

	/**
	 * @brief Cancel and close without applying.
	 */
	void cancelAndExit();

  protected:
	/**
	 * @brief Release octree resources.
	 */
	void releaseOctrees();

	/**
	 * @brief Update display.
	 *
	 * @param[in] showSF Show scalar field.
	 * @param[in] showRef Show reference entity.
	 */
	void updateDisplay(bool showSF, bool showRef);

	/**
	 * @brief Check if entities are valid.
	 * @return true if comparison is possible.
	 */
	bool isValid();

	/**
	 * @brief Prepare entities for comparison.
	 * @return true on success.
	 */
	bool prepareEntitiesForComparison();

	/**
	 * @brief Determine optimal octree level.
	 *
	 * @param[in] maxSearchDist Maximum search distance.
	 * @return Optimal octree subdivision level.
	 */
	int determineBestOctreeLevel(double maxSearchDist);

	/**
	 * @brief Compute approximate distances for preview.
	 * @return true on success.
	 */
	bool computeApproxDistances();

  protected slots:
	/**
	 * @brief Handle max distance update.
	 */
	void maxDistUpdated();

	/**
	 * @brief Enable 2D mode.
	 *
	 * @param[in] state 2D mode state.
	 */
	void enableCompute2D(bool state);

	/**
	 * @brief Get best octree level.
	 * @return Octree level.
	 */
	int getBestOctreeLevel();

	/**
	 * @brief Handle local model change.
	 *
	 * @param[in] index New model index.
	 */
	void locaModelChanged(int index);

	/**
	 * @brief Show distance histogram.
	 */
	void showHisto();

  protected:
	//! Compared entity
	ccHObject* m_compEnt = nullptr;

	//! Compared cloud
	ccPointCloud* m_compCloud = nullptr;

	//! Compared entity octree
	ccOctree::Shared m_compOctree;

	//! Whether compared octree is partial
	bool m_compOctreeIsPartial = false;

	//! Previous scalar field visibility
	bool m_compSFVisibility = false;

	//! Previous SF name
	QString m_oldSfName;

	//! Distance SF name
	QString m_sfName;

	//! Reference entity
	ccHObject* m_refEnt = nullptr;

	//! Reference cloud
	ccGenericPointCloud* m_refCloud = nullptr;

	//! Reference mesh
	ccGenericMesh* m_refMesh = nullptr;

	//! Reference entity octree
	ccOctree::Shared m_refOctree;

	//! Whether reference octree is partial
	bool m_refOctreeIsPartial = false;

	//! Previous reference visibility
	bool m_refVisibility = false;

	//! Comparison type
	CC_COMPARISON_TYPE m_compType = CLOUDCLOUD_DIST;

	//! No display mode
	bool m_noDisplay = false;

	//! Best octree level
	int m_bestOctreeLevel = 0;
};

#endif // CC_COMPARISON_DIALOG_HEADER
