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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccAboutDialog.cpp
 * @brief Implementation of the about dialog
 * @details Displays application version, compilation info, and credits.
 * @see ccAboutDialog
 */

#include "ccAboutDialog.h"

#include "ccApplication.h"
#include "ui_aboutDlg.h"

/**
 * @brief Constructor
 * @param parent Parent widget
 * @details Sets up the UI, retrieves version info, and enriches the
 * about text with compilation details (compiler, Qt version).
 */
ccAboutDialog::ccAboutDialog(QWidget* parent)
    : QDialog(parent)
    , mUI(new Ui::AboutDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);

	mUI->setupUi(this);

	QString compilationInfo;

	compilationInfo = ccApp->versionLongStr(true);
	compilationInfo += QStringLiteral("<br><i>Compiled with");

#if defined(_MSC_VER)
	compilationInfo += QStringLiteral(" MSVC %1 and").arg(_MSC_VER);
#endif

	compilationInfo += QStringLiteral(" Qt %1").arg(QT_VERSION_STR);
	compilationInfo += QStringLiteral("</i>");

	QString htmlText         = mUI->labelText->text();
	QString enrichedHtmlText = htmlText.arg(compilationInfo);

	mUI->labelText->setText(enrichedHtmlText);
}

/**
 * @brief Destructor
 * @details Deletes the UI structure.
 */
ccAboutDialog::~ccAboutDialog()
{
	delete mUI;
}
