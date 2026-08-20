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
 * @brief SOR filter dialog
 *
 * Dialog for Statistical Outlier Removal filter.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

namespace Ui
{
	class SorFilterDialog;
}

/**
 * @brief SOR filter dialog
 *
 * Configure Statistical Outlier Removal filter.
 */
class ccSORFilterDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccSORFilterDlg(QWidget* parent = nullptr);

	/// Destructor
	~ccSORFilterDlg();

	/// Get K nearest neighbors
	int KNN() const;
	/// Set K nearest neighbors
	void setKNN(int knn);

	/// Get n sigma
	double nSigma() const;
	/// Set n sigma
	void setNSigma(double nSigma);

	/// Set max thread count
	void setMaxThreadCount(int count);
	/// Get max thread count
	int maxThreadCount() const;

  private:
	Ui::SorFilterDialog* m_ui;
};

#endif // CC_SOR_FILTER_DLG_HEADER
