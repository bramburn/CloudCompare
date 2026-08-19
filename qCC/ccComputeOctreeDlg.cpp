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
 * @file ccComputeOctreeDlg.cpp
 * @brief Dialog for configuring octree computation parameters
 * @details Allows users to set octree subdivision mode: auto, by minimum
 * cell size, or by custom bounding box.
 * @see ccComputeOctreeDlg, ccOctree
 */

#include "ccComputeOctreeDlg.h"

// Local
#include "ccBoundingBoxEditorDlg.h"

// qCC_db
#include <ccOctree.h>

/**
 * @brief Constructor
 * @param baseBBox Base bounding box for reference
 * @param minCellSize Minimum allowed cell size
 * @param parent Parent widget
 */
ccComputeOctreeDlg::ccComputeOctreeDlg(const ccBBox& baseBBox, double minCellSize, QWidget* parent /*=nullptr*/)
    : QDialog(parent)
    , Ui::ComputeOctreeDialog()
    , m_bbEditorDlg(nullptr)
{
	setupUi(this);

	headerLabel->setText(QString("Max subdivision level: %1").arg(ccOctree::MAX_OCTREE_LEVEL));

	// minimum cell size
	if (minCellSize > 0.0)
	{
		cellSizeDoubleSpinBox->setMinimum(minCellSize);
		cellSizeDoubleSpinBox->setMaximum(1.0e9);
	}
	else
	{
		ccLog::Warning("[ccComputeOctreeDlg] Invalid minimum cell size specified!");
		cellSizeRadioButton->setEnabled(false);
	}

	// custom bbox editor
	if (baseBBox.isValid())
	{
		m_bbEditorDlg = new ccBoundingBoxEditorDlg(false, false, this);
		m_bbEditorDlg->setBaseBBox(baseBBox, true);
		m_bbEditorDlg->forceKeepSquare(true);
		connect(customBBToolButton, &QAbstractButton::clicked, m_bbEditorDlg, &ccBoundingBoxEditorDlg::exec);
	}
	else
	{
		ccLog::Warning("[ccComputeOctreeDlg] Invalid base bounding-box specified!");
		customBBRadioButton->setEnabled(false);
	}
}

/**
 * @brief Gets the selected computation mode
 * @return ComputationMode (DEFAULT, MIN_CELL_SIZE, or CUSTOM_BBOX)
 */
ccComputeOctreeDlg::ComputationMode ccComputeOctreeDlg::getMode() const
{
	// defaultRadioButton
	if (cellSizeRadioButton->isChecked())
		return MIN_CELL_SIZE;
	else if (customBBRadioButton->isChecked())
		return CUSTOM_BBOX;

	assert(defaultRadioButton->isChecked());
	return DEFAULT;
}

/**
 * @brief Gets the minimum cell size
 * @return Minimum cell size value
 */
double ccComputeOctreeDlg::getMinCellSize() const
{
	return cellSizeDoubleSpinBox->value();
}

/**
 * @brief Gets the custom bounding box
 * @return Custom bounding box if CUSTOM_BBOX mode selected
 */
ccBBox ccComputeOctreeDlg::getCustomBBox() const
{
	return (m_bbEditorDlg ? m_bbEditorDlg->getBox() : ccBBox());
}
