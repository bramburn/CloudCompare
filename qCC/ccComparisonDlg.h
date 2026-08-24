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
 * @brief Comparison dialog
 *
 * Dialog for cloud/cloud or cloud/mesh comparison.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
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
 * @brief Comparison dialog
 *
 * Configure cloud/cloud or cloud/mesh comparison.
 */
class ccComparisonDlg : public QDialog
    , public Ui::ComparisonDialog
{
	Q_OBJECT

  public:
	/// Comparison type
	enum CC_COMPARISON_TYPE
	{
		CLOUDCLOUD_DIST = 0,
		CLOUDMESH_DIST = 1,
	};

	/**
	 * @brief Create dialog
	 * @param[in] compEntity Entity to compare
	 * @param[in] refEntity Reference entity
	 * @param[in] cpType Comparison type
	 * @param[in] parent Parent widget
	 * @param[in] noDisplay No display mode
	 */
	ccComparisonDlg(ccHObject* compEntity,
	                ccHObject* refEntity,
	                CC_COMPARISON_TYPE cpType,
	                QWidget* parent = nullptr,
	                bool noDisplay = false);

	/// Destructor
	~ccComparisonDlg();

	/**
	 * @brief Initialize dialog
	 * @return true on success
	 */
	inline bool initDialog()
	{
		return computeApproxDistances();
	}

	/**
	 * @brief Get compared entity
	 * @return Compared entity
	 */
	ccHObject* getComparedEntity() const
	{
		return m_compEnt;
	}

	/**
	 * @brief Get reference entity
	 * @return Reference entity
	 */
	ccHObject* getReferenceEntity()
	{
		return m_refEnt;
	}

  public:
	bool computeDistances();
	void applyAndExit();
	void cancelAndExit();

  protected:
	// Internal helpers
	void releaseOctrees();
	void updateDisplay(bool showSF, bool showRef);
	bool isValid();
	bool prepareEntitiesForComparison();
	int determineBestOctreeLevel(double maxSearchDist);
	bool computeApproxDistances();

	// UI slots
	void maxDistUpdated();
	void enableCompute2D(bool state);
	int getBestOctreeLevel();
	void locaModelChanged(int index);
	void showHisto();

  protected:
	// Compared entity
	ccHObject* m_compEnt = nullptr;
	ccPointCloud* m_compCloud = nullptr;
	ccOctree::Shared m_compOctree;
	bool m_compOctreeIsPartial = false;
	bool m_compSFVisibility = false;
	QString m_oldSfName;
	QString m_sfName;

	// Reference entity
	ccHObject* m_refEnt = nullptr;
	ccGenericPointCloud* m_refCloud = nullptr;
	ccGenericMesh* m_refMesh = nullptr;
	ccOctree::Shared m_refOctree;
	bool m_refOctreeIsPartial = false;
	bool m_refVisibility = false;

	// Comparison config / state
	CC_COMPARISON_TYPE m_compType = CLOUDCLOUD_DIST;
	bool m_noDisplay = false;
	int m_bestOctreeLevel = 0;
};

#endif // CC_COMPARISON_DIALOG_HEADER
