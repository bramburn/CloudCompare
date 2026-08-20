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
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

#ifndef CC_STATISTICAL_TEST_DLG_HEADER
#define CC_STATISTICAL_TEST_DLG_HEADER

/**
 * @file ccStatisticalTestDlg.h
 *
 * @brief Statistical test dialog for local statistical analysis.
 *
 * @details Dialog for configuring local statistical test parameters.
 *
 * Statistical tests are used to:
 * - Detect outliers in point clouds
 * - Identify noise points
 * - Classify points based on local density
 *
 * Parameters:
 * - **Neighbors number**: How many nearest neighbors to consider
 * - **Probability**: Confidence level for the test
 * - **param1/2/3**: Test-specific parameters (e.g., sigma multipliers)
 *
 * Common uses:
 * - Local statistical outlier removal
 * - Noise filtering
 * - Anomaly detection
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

namespace Ui
{
	class StatisticalTestDialog;
}

/**
 * @brief Dialog for local statistical test configuration.
 *
 * @details Provides a UI for setting parameters used in local
 * statistical analysis algorithms.
 *
 * Features:
 * - Configurable neighbor count
 * - Probability/confidence setting
 * - Three customizable parameters
 *
 * @extends QDialog
 */
class ccStatisticalTestDlg : public QDialog
{
  public:
	/**
	 * @brief Construct the statistical test dialog.
	 *
	 * @param[in] param1Label Label for first parameter.
	 * @param[in] param2Label Label for second parameter.
	 * @param[in] param3Label Label for third parameter (optional).
	 * @param[in] windowTitle Window title (optional).
	 * @param[in] parent Parent widget.
	 */
	ccStatisticalTestDlg(QString  param1Label,
	                     QString  param2Label,
	                     QString  param3Label = QString(),
	                     QString  windowTitle = QString(),
	                     QWidget* parent      = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccStatisticalTestDlg();

	/**
	 * @brief Get parameter 1 value.
	 *
	 * @return Parameter 1.
	 */
	double getParam1() const;

	/**
	 * @brief Get parameter 2 value.
	 *
	 * @return Parameter 2.
	 */
	double getParam2() const;

	/**
	 * @brief Get parameter 3 value.
	 *
	 * @return Parameter 3.
	 */
	double getParam3() const;

	/**
	 * @brief Get number of neighbors.
	 *
	 * @return Neighbor count.
	 */
	int getNeighborsNumber() const;

	/**
	 * @brief Get probability value.
	 *
	 * @return Probability (0-1).
	 */
	double getProbability() const;

  private:
	//! UI definition
	Ui::StatisticalTestDialog* m_ui;
};

#endif // CC_STATISTICAL_TEST_DLG_HEADER
