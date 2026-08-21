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
 * @file ccLabelingDlg.cpp
 *
 * @brief Labeling dialog implementation
 *
 * Interactive dialog for adding 2D text labels to point clouds.
 * The user clicks in the 3D view to place labels at specific points.
 *
 * ## Labeling Workflow
 *
 * 1. Click in the 3D view to place a label at the clicked 3D position
 * 2. Enter the label text in the dialog
 * 3. Repeat for additional labels
 * 4. Confirm to create cc2DLabel entities in the db-tree
 *
 * Labels are stored as cc2DLabel objects with:
 * - Position in world coordinates
 * - Optional associated point index
 * - User-entered text
 *
 * @see ccLabelingDlg.h, cc2DLabel
 */

#include "ccLabelingDlg.h"

#include <DgmOctree.h>

ccLabelingDlg::ccLabelingDlg(QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::LabelingDialog()
{
	setupUi(this);

	octreeLevelSpinBox->setMaximum(CCCoreLib::DgmOctree::MAX_OCTREE_LEVEL);
}

int ccLabelingDlg::getOctreeLevel()
{
	return octreeLevelSpinBox->value();
}

int ccLabelingDlg::getMinPointsNb()
{
	return minPtsSpinBox->value();
}

bool ccLabelingDlg::randomColors()
{
	return (randomColorsCheckBox->checkState() == Qt::Checked);
}

void ccLabelingDlg::setOctreeLevel(int octreeLevel)
{
	octreeLevelSpinBox->setValue(octreeLevel);
}

void ccLabelingDlg::setMinPointsNb(int minPointsNb)
{
	minPtsSpinBox->setValue(minPointsNb);
}

void ccLabelingDlg::setRandomColors(bool state)
{
	randomColorsCheckBox->setChecked(state);
}
