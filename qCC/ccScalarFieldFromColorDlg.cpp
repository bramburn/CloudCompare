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
 * @file ccScalarFieldFromColorDlg.cpp
 *
 * @brief Scalar field from color dialog implementation
 *
 * Dialog for extracting scalar fields from RGB(A) color channels
 * of a point cloud. Each selected channel creates a new scalar field
 * with values normalized to [0, 255].
 *
 * ## Output Scalar Fields
 *
 * - **R/G/B**: single channel as grayscale scalar field
 * - **Alpha**: transparency channel as scalar field
 * - **Composite**: weighted combination (e.g., luminance)
 *
 * @see ccScalarFieldFromColorDlg.h
 */

#include "ccScalarFieldFromColorDlg.h"

// Qt
#include <QPushButton>

// qCC_db
#include <assert.h>
#include <ccPointCloud.h>
#ifdef _MSC_VER
#include <windows.h>
#endif

ccScalarFieldFromColorDlg::ccScalarFieldFromColorDlg(QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::scalarFieldFromColorDlg()
{
	setupUi(this);
}

bool ccScalarFieldFromColorDlg::getRStatus() const
{
	return this->checkBoxR->isChecked();
}

bool ccScalarFieldFromColorDlg::getGStatus() const
{
	return this->checkBoxG->isChecked();
}

bool ccScalarFieldFromColorDlg::getBStatus() const
{
	return this->checkBoxB->isChecked();
}

bool ccScalarFieldFromColorDlg::getAlphaStatus() const
{
	return this->checkBoxA->isChecked();
}

bool ccScalarFieldFromColorDlg::getCompositeStatus() const
{
	return this->checkBoxComposite->isChecked();
}
