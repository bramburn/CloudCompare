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
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

// Local
/**
 * @file ccGraphicalSegmentationOptionsDlg.cpp
 *
 * @brief Segmentation options dialog implementation
 *
 * Dialog for configuring graphical segmentation tool options.
 *
 * ## Segmentation Options
 *
 * - **Polyline mode**: segment by drawing closed polygons in the 3D view
 * - **Rectangle mode**: segment by drawing bounding rectangles
 * - **Slice mode**: segment by drawing planar slices
 * - **Height filter**: segment by Z-coordinate threshold
 *
 * Settings are persisted to QSettings for session continuity.
 *
 * @see ccGraphicalSegmentationOptionsDlg.h
 */

#include "ccGraphicalSegmentationOptionsDlg.h"

// Qt
#include <QSettings>

ccGraphicalSegmentationOptionsDlg::ccGraphicalSegmentationOptionsDlg(const QString windowTitle /*=QString()*/,
                                                                     QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , Ui::GraphicalSegmentationOptionsDlg()
{
	setupUi(this);

	QSettings settings;
	settings.beginGroup(SegmentationToolOptionsKey());
	QString remainingSuffix = settings.value(RemainingSuffixKey(), ".remaining").toString();
	QString segmentedSuffix = settings.value(SegmentedSuffixKey(), ".segmented").toString();
	settings.endGroup();

	remainingTextLineEdit->setText(remainingSuffix);
	segmentedTextLineEdit->setText(segmentedSuffix);

	if (!windowTitle.isEmpty())
	{
		setWindowTitle(windowTitle);
	}
}

void ccGraphicalSegmentationOptionsDlg::accept()
{
	QSettings settings;
	settings.beginGroup(SegmentationToolOptionsKey());
	settings.setValue(RemainingSuffixKey(), remainingTextLineEdit->text());
	settings.setValue(SegmentedSuffixKey(), segmentedTextLineEdit->text());
	settings.endGroup();

	QDialog::accept();
}
