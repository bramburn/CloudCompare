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

#ifndef CC_STATISTICAL_TEST_DLG_HEADER
#define CC_STATISTICAL_TEST_DLG_HEADER

/**
 * @file ccStatisticalTestDlg.h
 *
 * @brief Statistical test dialog
 *
 * Dialog for local statistical test tool.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

namespace Ui
{
	class StatisticalTestDialog;
}

/**
 * @brief Statistical test dialog
 *
 * Configure local statistical test parameters.
 */
class ccStatisticalTestDlg : public QDialog
{
  public:
	/**
	 * @brief Create dialog
	 * @param[in] param1Label Label for parameter 1
	 * @param[in] param2Label Label for parameter 2
	 * @param[in] param3Label Label for parameter 3
	 * @param[in] windowTitle Window title
	 * @param[in] parent Parent widget
	 */
	ccStatisticalTestDlg(QString param1Label,
	                     QString param2Label,
	                     QString param3Label = QString(),
	                     QString windowTitle = QString(),
	                     QWidget* parent = nullptr);

	/// Destructor
	~ccStatisticalTestDlg();

	/// Get parameter 1
	double getParam1() const;
	/// Get parameter 2
	double getParam2() const;
	/// Get parameter 3
	double getParam3() const;

	/// Get neighbors number
	int getNeighborsNumber() const;
	/// Get probability
	double getProbability() const;

  private:
	Ui::StatisticalTestDialog* m_ui;
};

#endif // CC_STATISTICAL_TEST_DLG_HEADER
