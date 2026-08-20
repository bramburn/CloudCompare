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

#ifndef CC_REGISTRATION_DLG_HEADER
#define CC_REGISTRATION_DLG_HEADER

/**
 * @file ccRegistrationDlg.h
 *
 * @brief Registration dialog for ICP-based point cloud alignment.
 *
 * @details Dialog for configuring ICP (Iterative Closest Point) registration
 * parameters.
 *
 * ICP registration iteratively aligns two point clouds by:
 * 1. Finding corresponding point pairs
 * 2. Computing optimal transformation
 * 3. Applying transformation and repeating
 *
 * Parameters include:
 * - Convergence criteria (max iterations, RMS threshold)
 * - Sampling options (random sampling, farthest point removal)
 * - Transformation model (rigid, similarity)
 * - Weight options (scalar field weights)
 * - Normals matching strategy
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccRegistrationTools
 * @see CCCoreLib::ICPRegistrationTools
 */

#include <QDialog>

// CCCoreLib
#include <ReferenceCloud.h>
#include <RegistrationTools.h>
#include <ui_registrationDlg.h>

class ccHObject;

/**
 * @brief Dialog for configuring ICP registration parameters.
 *
 * @details Provides a UI for setting up ICP registration parameters.
 *
 * Convergence options:
 * - Max iterations: Stop after N iterations
 * - RMS decrease: Stop when improvement is below threshold
 *
 * Sampling options:
 * - Random sampling limit for performance
 * - Farthest point removal for robustness
 *
 * @extends QDialog
 * @extends Ui::RegistrationDialog
 */
class ccRegistrationDlg : public QDialog
    , public Ui::RegistrationDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Convergence method types.
	 */
	typedef CCCoreLib::ICPRegistrationTools::CONVERGENCE_TYPE ConvergenceMethod;

	/**
	 * @brief Construct the registration dialog.
	 *
	 * @param[in] data Data cloud (will be transformed).
	 * @param[in] model Reference model cloud.
	 * @param[in] parent Parent widget.
	 */
	ccRegistrationDlg(ccHObject* data, ccHObject* model, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccRegistrationDlg();

	/**
	 * @brief Get convergence method.
	 * @return Convergence method.
	 */
	ConvergenceMethod getConvergenceMethod() const;

	/**
	 * @brief Get maximum iteration count.
	 * @return Max iterations.
	 *
	 * @note Only applicable for ITERATION_REG convergence.
	 */
	unsigned getMaxIterationCount() const;

	/**
	 * @brief Get final overlap percentage.
	 * @return Overlap (0-100).
	 */
	unsigned getFinalOverlap() const;

	/**
	 * @brief Get minimum RMS decrease threshold.
	 * @return Minimum RMS decrease.
	 *
	 * @note Only applicable for MAX_ERROR_REG convergence.
	 */
	double getMinRMSDecrease() const;

	/**
	 * @brief Get absolute minimum RMS decrease.
	 * @return Absolute minimum value.
	 */
	static double GetAbsoluteMinRMSDecrease();

	/**
	 * @brief Set minimum RMS decrease.
	 * @param[in] value New threshold.
	 */
	void setMinRMSDecrease(double value);

	/**
	 * @brief Check if farthest points should be removed.
	 * @return true if removal is enabled.
	 */
	bool removeFarthestPoints() const;

	/**
	 * @brief Get random sampling limit.
	 * @return Maximum random sample size.
	 */
	unsigned randomSamplingLimit() const;

	/**
	 * @brief Get the model entity.
	 * @return Model/reference entity.
	 */
	ccHObject* getModelEntity();

	/**
	 * @brief Get the data entity.
	 * @return Data entity to be transformed.
	 */
	ccHObject* getDataEntity();

	/**
	 * @brief Check if data SF is used as weights.
	 * @return true if data weights enabled.
	 */
	bool useDataSFAsWeights() const;

	/**
	 * @brief Check if model SF is used as weights.
	 * @return true if model weights enabled.
	 */
	bool useModelSFAsWeights() const;

	/**
	 * @brief Get cloud-to-mesh signed distances option.
	 *
	 * @param[out] robust Use robust fitting.
	 * @return true if C2M signed distances enabled.
	 */
	bool useC2MSignedDistances(bool& robust) const;

	/**
	 * @brief Get normals matching option.
	 * @return Normals matching strategy.
	 */
	CCCoreLib::ICPRegistrationTools::NORMALS_MATCHING normalsMatchingOption() const;

	/**
	 * @brief Check if scale adjustment is enabled.
	 * @return true if scale adjustment is on.
	 */
	bool adjustScale() const;

	/**
	 * @brief Get maximum thread count.
	 * @return Thread count.
	 */
	int getMaxThreadCount() const;

	/**
	 * @brief Get transformation filter flags.
	 * @return Filter flags.
	 */
	int getTransformationFilters() const;

	/**
	 * @brief Update GUI state.
	 */
	void updateGUI();

	/**
	 * @brief Swap model and data entities.
	 */
	void swapModelAndData();

	/**
	 * @brief Save parameters for next session.
	 */
	void saveParameters() const;

  protected:
	//! Data entity
	ccHObject* dataEntity = nullptr;

	//! Model entity
	ccHObject* modelEntity = nullptr;

	//! Error criterion radio button
	QRadioButton* errorCriterion = nullptr;
};

#endif // CC_REGISTRATION_DLG_HEADER
