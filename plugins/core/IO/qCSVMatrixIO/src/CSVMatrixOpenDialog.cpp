// ##########################################################################
// #                                                                        #
// #                  CLOUDCOMPARE PLUGIN: qCSVMatrixIO                     #
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
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

/**
 * @file CSVMatrixOpenDialog.cpp
 *
 * @brief CSV matrix open dialog implementation
 *
 * Dialog for importing a matrix stored as a CSV file:
 * - Rows × Columns table
 * - Optional header row/column for labels
 * - Delimiter detection (comma, semicolon, tab)
 *
 * @see CSVMatrixFilter.h
 */
#include "CSVMatrixOpenDialog.h"

#include "ccFileUtils.h"

// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

CSVMatrixOpenDialog::CSVMatrixOpenDialog(QWidget* parent /*=nullptr*/)
    : QDialog(parent)
    , Ui::CSVMatrixOpenDlg()
{
	setupUi(this);

	connect(browseToolButton, &QAbstractButton::clicked, this, &CSVMatrixOpenDialog::browseTextureFile);

	// persistent settings
	QSettings settings;
	settings.beginGroup("LoadFile");
	QString currentPath = settings.value("currentPath", ccFileUtils::defaultDocPath()).toString();

	textureFilenameLineEdit->setText(currentPath);
}

void CSVMatrixOpenDialog::browseTextureFile()
{
	QString inputFilename = QFileDialog::getOpenFileName(this, "Texture file", textureFilenameLineEdit->text(), "*.*");
	if (inputFilename.isEmpty())
		return;

	textureFilenameLineEdit->setText(inputFilename);

	// save last loading location
	QSettings settings;
	settings.beginGroup("LoadFile");
	settings.setValue("currentPath", QFileInfo(inputFilename).absolutePath());
	settings.endGroup();
}
