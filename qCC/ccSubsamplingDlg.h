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
// #                                                                        //
// ##########################################################################

/**
 * @file ccSubsamplingDlg.h
 *
 * @brief Subsampling dialog for point cloud reduction.
 *
 * @details Dialog for configuring point cloud subsampling (reduction) methods.
 *
 * Subsampling reduces the number of points while trying to preserve
 * the overall shape and characteristics of the cloud.
 *
 * Methods:
 * - **RANDOM**: Randomly select N points
 * - **RANDOM_PERCENT**: Randomly select X% of points
 * - **SPATIAL**: Space-based sampling (grid subdivision)
 * - **OCTREE**: Octree-based sampling (cell centers)
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

#include <CCTypes.h>

// System
#include <array>

class ccGenericPointCloud;

namespace CCCoreLib
{
	class GenericProgressCallback;
	class ReferenceCloud;
} // namespace CCCoreLib

namespace Ui
{
	class SubsamplingDialog;
}

/**
 * @brief Dialog for point cloud subsampling.
 *
 * @details Provides a UI for setting up point cloud subsampling parameters.
 *
 * Features:
 * - Multiple sampling methods
 * - Parameter adjustment with sliders
 * - SF (scalar field) modulation for SPATIAL method
 * - Persistent settings (remembers last used parameters)
 *
 * @extends QDialog
 */
class ccSubsamplingDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Subsampling methods.
	 */
	enum CC_SUBSAMPLING_METHOD
	{
		RANDOM         = 0, //!< Random point selection
		RANDOM_PERCENT = 1, //!< Random percentage selection
		SPATIAL        = 2, //!< Space-based grid sampling
		OCTREE         = 3, //!< Octree cell center sampling
		COUNT          = 4  //!< Number of methods
	};

	/**
	 * @brief Construct the subsampling dialog.
	 *
	 * @param[in] maxPointCount Maximum point count (for RANDOM method).
	 * @param[in] maxCloudRadius Maximum cloud radius.
	 * @param[in] parent Parent widget.
	 */
	ccSubsamplingDlg(unsigned maxPointCount, double maxCloudRadius, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccSubsamplingDlg() override;

	/**
	 * @brief Get the subsampled cloud.
	 *
	 * @param[in] cloud Cloud to subsample.
	 * @param[in] progressCb Progress callback.
	 *
	 * @return Subsampled cloud, or nullptr if cancelled.
	 *
	 * @note Should only be called after dialog is accepted.
	 * @note The returned ReferenceCloud must be deleted by caller.
	 */
	CCCoreLib::ReferenceCloud* getSampledCloud(ccGenericPointCloud* cloud, CCCoreLib::GenericProgressCallback* progressCb = nullptr);

	/**
	 * @brief Enable SF modulation for SPATIAL method.
	 *
	 * @param[in] sfMin Minimum SF value.
	 * @param[in] sfMax Maximum SF value.
	 */
	void enableSFModulation(ScalarType sfMin, ScalarType sfMax);

	/**
	 * @brief Save state to persistent settings.
	 */
	void saveToPersistentSettings() const;

	/**
	 * @brief Load state from persistent settings.
	 */
	void loadFromPersistentSettings();

  protected slots:
	/**
	 * @brief Handle slider movement.
	 * @param[in] sliderPos New slider position.
	 */
	void sliderMoved(int sliderPos);

	/**
	 * @brief Handle value change.
	 * @param[in] value New value.
	 */
	void valueChanged(double value);

	/**
	 * @brief Handle method change.
	 * @param[in] index New method index.
	 */
	void changeSamplingMethod(int index);

  protected:
	/**
	 * @brief Update labels based on current method.
	 */
	void updateLabels();

  protected: // members
	//! Maximum point count
	unsigned m_maxPointCount;

	//! Maximum cloud radius
	double m_maxRadius;

	//! SF modulation enabled
	bool m_sfModEnabled;

	//! SF minimum
	ScalarType m_sfMin;

	//! SF maximum
	ScalarType m_sfMax;

	//! Last used values per method
	std::array<double, CC_SUBSAMPLING_METHOD::COUNT> m_lastUsedValues;

	//! UI definition
	Ui::SubsamplingDialog* m_ui;
};
