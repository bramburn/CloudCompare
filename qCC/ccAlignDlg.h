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

/**
 * @file ccAlignDlg.h
 *
 * @brief Alignment dialog for rough point cloud registration.
 *
 * @details Dialog for performing rough point cloud registration
 * (alignment) before fine registration with ICP.
 *
 * The alignment algorithm:
 * 1. Samples points from both clouds using the selected method
 * 2. Finds corresponding point pairs
 * 3. Computes the transformation matrix
 *
 * This dialog sets up parameters for the coarse alignment phase.
 * After alignment, ICP (Iterative Closest Point) can be used
 * for fine registration.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGraphicalTransformationTool
 * @see CCCoreLib::ICPRegistrationTools
 */

#include <QDialog>

class ccGenericPointCloud;

namespace CCCoreLib
{
	class ReferenceCloud;
}

namespace Ui
{
	class AlignDialog;
}

/**
 * @brief Alignment dialog for rough point cloud registration.
 *
 * @details Provides a dialog for setting up coarse point cloud alignment
 * parameters. The user specifies:
 * - Sampling method and rate for both clouds
 * - Expected overlap percentage
 * - Maximum number of candidate alignments to try
 * - Distance threshold (delta)
 *
 * The dialog outputs sampled point sets that can be fed to the
 * alignment algorithm. This is typically the first step in
 * registering two point clouds, followed by ICP for fine-tuning.
 *
 * @extends QDialog
 */
class ccAlignDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Point sampling methods for alignment.
	 */
	enum CC_SAMPLING_METHOD
	{
		NONE = 0,  //!< Use all points
		RANDOM,     //!< Random sampling
		SPACE,      //!< Space-based sampling
		OCTREE      //!< Octree-based sampling
	};

	/**
	 * @brief Construct the alignment dialog.
	 *
	 * @param[in] data The "data" cloud (will be transformed).
	 * @param[in] model The "model" cloud (reference, stays fixed).
	 * @param[in] parent Parent widget.
	 *
	 * @details Initializes the dialog with the two clouds to align.
	 * The data cloud will be moved to match the model cloud.
	 */
	ccAlignDlg(ccGenericPointCloud* data, ccGenericPointCloud* model, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccAlignDlg();

	/**
	 * @brief Get the number of alignment attempts.
	 *
	 * @return Number of times to try finding the best alignment.
	 */
	unsigned getNbTries();

	/**
	 * @brief Get the expected overlap percentage.
	 *
	 * @return Overlap as a percentage (0-100).
	 *
	 * @details Estimated overlap between the two clouds.
	 * Higher overlap = more reliable alignment.
	 */
	double getOverlap();

	/**
	 @brief Get the distance threshold (delta).
	 *
	 @return Maximum distance for finding correspondences.
	 */
	double getDelta();

	/**
	 * @brief Get the model (reference) cloud.
	 *
	 * @return Pointer to the model cloud.
	 */
	ccGenericPointCloud* getModelObject();

	/**
	 * @brief Get the data (to-align) cloud.
	 *
	 * @return Pointer to the data cloud.
	 */
	ccGenericPointCloud* getDataObject();

	/**
	 * @brief Get the sampling method.
	 *
	 * @return Sampling method enum.
	 */
	CC_SAMPLING_METHOD getSamplingMethod();

	/**
	 * @brief Check if number of candidates is limited.
	 *
	 * @return true if max number of candidates is set.
	 */
	bool isNumberOfCandidatesLimited();

	/**
	 * @brief Get the maximum number of candidates.
	 *
	 * @return Maximum number of alignment candidates to try.
	 */
	unsigned getMaxNumberOfCandidates();

	/**
	 * @brief Get the sampled model cloud.
	 *
	 * @return ReferenceCloud with sampled model points.
	 *
	 * @note The returned ReferenceCloud must be deleted by caller.
	 */
	CCCoreLib::ReferenceCloud* getSampledModel();

	/**
	 * @brief Get the sampled data cloud.
	 *
	 * @return ReferenceCloud with sampled data points.
	 *
	 * @note The returned ReferenceCloud must be deleted by caller.
	 */
	CCCoreLib::ReferenceCloud* getSampledData();

  protected slots:
	/**
	 * @brief Swap model and data clouds.
	 *
	 * @details Switches which cloud is considered the model
	 * and which is the data. Useful if alignment is failing
	 * in the opposite direction.
	 */
	void swapModelAndData();

	/**
	 * @brief Handle model slider release.
	 *
	 * @details Updates the model cloud sampling rate
	 * when the user finishes dragging the slider.
	 */
	void modelSliderReleased();

	/**
	 * @brief Handle data slider release.
	 */
	void dataSliderReleased();

	/**
	 * @brief Handle model sampling rate change.
	 *
	 * @param[in] value New sampling rate.
	 */
	void modelSamplingRateChanged(double value);

	/**
	 * @brief Handle data sampling rate change.
	 *
	 * @param[in] value New sampling rate.
	 */
	void dataSamplingRateChanged(double value);

	/**
	 * @brief Estimate the distance threshold automatically.
	 *
	 * @details Attempts to compute a reasonable delta value
	 * based on cloud characteristics.
	 */
	void estimateDelta();

	/**
	 * @brief Change the sampling method.
	 *
	 * @param[in] index Combo box index.
	 */
	void changeSamplingMethod(int index);

	/**
	 * @brief Toggle limitation of candidate count.
	 *
	 * @param[in] activ Whether to limit candidates.
	 */
	void toggleNbMaxCandidates(bool activ);

  protected:
	/**
	 * @brief Set colors and labels for clouds.
	 *
	 * @details Updates the UI to show the cloud names
	 * and appropriate colors.
	 */
	void setColorsAndLabels();

	//! The model (reference) cloud - stays fixed
	ccGenericPointCloud* modelObject;

	//! The data cloud - will be transformed
	ccGenericPointCloud* dataObject;

	//! UI definition from .ui file
	Ui::AlignDialog* m_ui;
};
