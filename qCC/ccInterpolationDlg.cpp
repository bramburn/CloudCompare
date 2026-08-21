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
// #                    COPYRIGHT: Daniel Girardeau-Montaut                 #
// #                                                                        #
// ##########################################################################

/**
 * @file ccInterpolationDlg.cpp
 *
 * @brief Interpolation dialog implementation
 *
 * Dialog for configuring point cloud interpolation parameters.
 *
 * ## Interpolation Parameters
 *
 * - **Method**: interpolation strategy (e.g., natural neighbor, IDW, kriging)
 * - **Algorithm**: specific algorithm implementation
 * - **Resolution**: output grid cell size
 * - **Max distance**: max search radius for neighborhood
 * - **Min neighbors**: minimum neighbors required for interpolation
 *
 * Used with ccPointCloudInterpolator to resample a sparse point cloud
 * onto a regular grid.
 *
 * @see ccInterpolationDlg.h, ccPointCloudInterpolator
 */

#include "ccInterpolationDlg.h"

// System
#include <assert.h>

ccInterpolationDlg::ccInterpolationDlg(QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::InterpolationDlg()
{
	setupUi(this);

	connect(radiusDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccInterpolationDlg::onRadiusUpdated);
}

ccPointCloudInterpolator::Parameters::Method ccInterpolationDlg::getInterpolationMethod() const
{
	if (nnRadioButton->isChecked())
		return ccPointCloudInterpolator::Parameters::NEAREST_NEIGHBOR;
	else if (radiusRadioButton->isChecked())
		return ccPointCloudInterpolator::Parameters::RADIUS;
	else if (knnRadioButton->isChecked())
		return ccPointCloudInterpolator::Parameters::K_NEAREST_NEIGHBORS;

	assert(false);
	return ccPointCloudInterpolator::Parameters::NEAREST_NEIGHBOR;
}

void ccInterpolationDlg::setInterpolationMethod(ccPointCloudInterpolator::Parameters::Method method)
{
	switch (method)
	{
	case ccPointCloudInterpolator::Parameters::NEAREST_NEIGHBOR:
		nnRadioButton->setChecked(true);
		break;
	case ccPointCloudInterpolator::Parameters::RADIUS:
		radiusRadioButton->setChecked(true);
		break;
	case ccPointCloudInterpolator::Parameters::K_NEAREST_NEIGHBORS:
		knnRadioButton->setChecked(true);
		break;
	default:
		assert(false);
	}
}

ccPointCloudInterpolator::Parameters::Algo ccInterpolationDlg::getInterpolationAlgorithm() const
{
	if (averageRadioButton->isChecked())
		return ccPointCloudInterpolator::Parameters::AVERAGE;
	else if (medianRadioButton->isChecked())
		return ccPointCloudInterpolator::Parameters::MEDIAN;
	else if (normalDistribRadioButton->isChecked())
		return ccPointCloudInterpolator::Parameters::NORMAL_DIST;

	assert(false);
	return ccPointCloudInterpolator::Parameters::AVERAGE;
}

void ccInterpolationDlg::setInterpolationAlgorithm(ccPointCloudInterpolator::Parameters::Algo algo)
{
	switch (algo)
	{
	case ccPointCloudInterpolator::Parameters::AVERAGE:
		averageRadioButton->setChecked(true);
		break;
	case ccPointCloudInterpolator::Parameters::MEDIAN:
		medianRadioButton->setChecked(true);
		break;
	case ccPointCloudInterpolator::Parameters::NORMAL_DIST:
		normalDistribRadioButton->setChecked(true);
		break;
	default:
		assert(false);
	}
}

void ccInterpolationDlg::setNoNormalization(bool state)
{
	noNormalizationCheckBox->setChecked(state);
}

bool ccInterpolationDlg::noNormalization() const
{
	return noNormalizationCheckBox->isChecked();
}

void ccInterpolationDlg::onRadiusUpdated(double radius)
{
	kernelDoubleSpinBox->setValue(radius / 2.5);
}
