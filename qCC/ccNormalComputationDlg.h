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

#ifndef CC_NORMAL_COMPUTATION_DLG_HEADER
#define CC_NORMAL_COMPUTATION_DLG_HEADER

/**
 * @file ccNormalComputationDlg.h
 *
 * @brief Normal computation dialog for configuring normal estimation.
 *
 * @details Dialog for setting up parameters for point cloud normal
 * computation and orientation.
 *
 * Features:
 * - Local model selection (plane, quadric, etc.)
 * - Neighborhood radius configuration
 * - Normal orientation options
 * - Scan grid and sensor support
 *
 * Normal computation uses local surface fitting to estimate
 * the normal direction at each point.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccNormalVectors
 * @see CCCoreLib::LOCAL_MODEL_TYPES
 */

// qCC_db
#include <ccNormalVectors.h>

class ccPointCloud;

/**
 * @brief Dialog for configuring normal computation parameters.
 *
 * @details Provides a UI for setting up normal estimation parameters.
 *
 * Parameters include:
 * - **Local model**: Surface type to fit (plane, quadric, etc.)
 * - **Radius**: Neighborhood size for local fitting
 * - **Orientation**: How to orient normals (MST, scan grids, sensors)
 * - **Preferred direction**: Base orientation for consistent normals
 *
 * @extends QDialog
 * @extends Ui::NormalComputationDlg
 */
class ccNormalComputationDlg : public QDialog
    , public Ui::NormalComputationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the normal computation dialog.
	 *
	 * @param[in] withScanGrid Whether selection contains structured clouds.
	 * @param[in] withSensor Whether selection contains sensors.
	 * @param[in] parent Parent widget.
	 */
	explicit ccNormalComputationDlg(bool withScanGrid, bool withSensor, QWidget* parent = nullptr);

	/**
	 * @brief Get the local model type.
	 * @return Local surface model type.
	 */
	CCCoreLib::LOCAL_MODEL_TYPES getLocalModel() const;

	/**
	 * @brief Set the local model type.
	 * @param[in] model Model type to use.
	 */
	void setLocalModel(CCCoreLib::LOCAL_MODEL_TYPES model);

	/**
	 * @brief Set the default radius.
	 * @param[in] radius Neighborhood radius.
	 */
	void setRadius(PointCoordinateType radius);

	/**
	 * @brief Set the preferred orientation.
	 * @param[in] orientation Preferred direction.
	 */
	void setPreferredOrientation(ccNormalVectors::Orientation orientation);

	/**
	 * @brief Set the cloud for auto radius estimation.
	 * @param[in] cloud Cloud to analyze.
	 */
	void setCloud(ccPointCloud* cloud);

	/**
	 * @brief Check if scan grids should be used for computation.
	 * @return true if scan grids should be used.
	 */
	bool useScanGridsForComputation() const;

	/**
	 * @brief Get the minimum grid triangle angle.
	 * @return Angle in degrees.
	 */
	double getMinGridAngle_deg() const;

	/**
	 * @brief Set the minimum grid triangle angle.
	 * @param[in] value Angle in degrees.
	 */
	void setMinGridAngle_deg(double value);

	/**
	 * @brief Get the neighborhood radius.
	 * @return Radius value.
	 */
	PointCoordinateType getRadius() const;

	/**
	 * @brief Check if normals should be oriented.
	 * @return true if orientation should be applied.
	 */
	bool orientNormals() const;

	/**
	 @brief Set whether to orient normals.
	 @param[in] state Orientation state.
	 */
	void setOrientNormals(bool state);

	/**
	 * @brief Check if scan grids should be used for orientation.
	 * @return true if scan grids should be used.
	 */
	bool useScanGridsForOrientation() const;

	/**
	 * @brief Check if sensors should be used for orientation.
	 * @return true if sensors should be used.
	 */
	bool useSensorsForOrientation() const;

	/**
	 * @brief Check if preferred orientation should be used.
	 * @return true if preferred orientation is enabled.
	 */
	bool usePreferredOrientation() const;

	/**
	 * @brief Get the preferred orientation.
	 * @return Orientation direction.
	 */
	ccNormalVectors::Orientation getPreferredOrientation() const;

	/**
	 * @brief Check if MST should be used for orientation.
	 * @return true if Minimum Spanning Tree should be used.
	 */
	bool useMSTOrientation() const;

	/**
	 * @brief Get the MST neighbor count.
	 * @return Number of neighbors.
	 */
	int getMSTNeighborCount() const;

	/**
	 * @brief Set the MST neighbor count.
	 * @param[in] n Number of neighbors.
	 */
	void setMSTNeighborCount(int n);

  protected slots:
	/**
	 * @brief Handle local model change.
	 * @param[in] index New model index.
	 */
	void localModelChanged(int index);

	/**
	 * @brief Auto-estimate the radius.
	 */
	void autoEstimateRadius();

  protected:
	//! Cloud for auto estimation
	ccPointCloud* m_cloud;
};

#endif // CC_NORMAL_COMPUTATION_DLG_HEADER
