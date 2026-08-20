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

#ifndef CC_SOR_FILTER_DLG_HEADER
#define CC_SOR_FILTER_DLG_HEADER

/**
 * @file ccSORFilterDlg.h
 *
 * @brief Statistical Outlier Removal filter dialog.
 *
 * @details Dialog for configuring the Statistical Outlier Removal (SOR)
 * filter parameters.
 *
 * SOR is a point cloud filtering method that removes outlier points
 * based on statistical analysis of neighbor distances:
 *
 * 1. For each point, compute distances to its K nearest neighbors
 * 2. Calculate the mean and standard deviation of these distances
 * 3. Points with mean distance > (mean + n*stddev) are outliers
 * 4. Remove or mark outliers
 *
 * Parameters:
 * - **K nearest neighbors**: Number of neighbors to analyze
 * - **n sigma**: Number of standard deviations for threshold
 * - **Max thread count**: Parallelization limit
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEntityAction::filterBySF()
 */

#include <QDialog>

namespace Ui
{
	class SorFilterDialog;
}

/**
 * @brief Dialog for Statistical Outlier Removal filter.
 *
 * @details Provides a UI for setting up SOR filter parameters.
 *
 * The Statistical Outlier Removal filter:
 * - Computes statistics on local point density
 * - Removes points that are isolated from their neighbors
 * - Preserves the overall structure while removing noise
 *
 * @extends QDialog
 */
class ccSORFilterDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the SOR filter dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccSORFilterDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccSORFilterDlg();

	/**
	 * @brief Get K nearest neighbors count.
	 * @return Number of neighbors to analyze.
	 */
	int KNN() const;

	/**
	 * @brief Set K nearest neighbors count.
	 * @param[in] knn Number of neighbors.
	 */
	void setKNN(int knn);

	/**
	 * @brief Get n sigma threshold.
	 * @return Number of standard deviations.
	 */
	double nSigma() const;

	/**
	 * @brief Set n sigma threshold.
	 * @param[in] nSigma Number of standard deviations.
	 */
	void setNSigma(double nSigma);

	/**
	 * @brief Set maximum thread count.
	 * @param[in] count Maximum threads.
	 */
	void setMaxThreadCount(int count);

	/**
	 * @brief Get maximum thread count.
	 * @return Maximum thread count.
	 */
	int maxThreadCount() const;

  private:
	//! UI definition
	Ui::SorFilterDialog* m_ui;
};

#endif // CC_SOR_FILTER_DLG_HEADER
