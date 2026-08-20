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
 *
 * @brief Implementation of the about dialog.
 *
 * @details Implements the ccAboutDialog class for displaying
 * CloudCompare application information.
 *
 * @see ccAboutDialog
 */

#include "ccAboutDialog.h"

#include "ccApplication.h"
#include "ui_aboutDlg.h"

/**
 * @brief Construct the about dialog.
 *
 * @param[in] parent Parent widget.
 *
 * @details Sets up the UI from the .ui file and enriches the displayed
 * text with compilation information including:
 * - Full version string (from ccApplication)
 * - Compiler version (e.g., MSVC 1939)
 * - Qt version
 *
 * The dialog text is enriched by replacing placeholders in the HTML
 * template with actual compilation details.
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
 * @brief Destructor.
 *
 * @details Cleans up the UI structure created by setupUi().
 */
ccAboutDialog::~ccAboutDialog()
{
	delete mUI;
}
