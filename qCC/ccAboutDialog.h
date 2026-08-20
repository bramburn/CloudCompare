#ifndef CCABOUTDIALOG_H
#define CCABOUTDIALOG_H

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
// #          COPYRIGHT: CloudCompare project                               //
// #                                                                        #
// ##########################################################################

/**
 * @file ccAboutDialog.h
 *
 * @brief About dialog for CloudCompare.
 *
 * @details Displays the application "About" dialog showing:
 * - Application name and version
 * - Copyright information
 * - Compilation details (compiler, Qt version)
 * - Credits and acknowledgments
 *
 * @author CloudCompare project
 *
 * @see ccAboutDialog
 * @see HelpMenu
 */

#include <QDialog>

namespace Ui
{
	class AboutDialog;
}

/**
 * @brief About dialog for CloudCompare.
 *
 * @details Shows the application information dialog. The dialog is
 * automatically deleted when closed (Qt::WA_DeleteOnClose attribute).
 *
 * The dialog displays information from the application's .ui file
 * and enriches it with compilation details retrieved from the
 * application object.
 *
 * @extends QDialog
 *
 * @par Usage
 * @code
 * ccAboutDialog* about = new ccAboutDialog(this);
 * about->setAttribute(Qt::WA_DeleteOnClose);
 * about->show();
 * @endcode
 */
class ccAboutDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the about dialog.
	 *
	 * @param[in] parent Parent widget (typically the main window).
	 *
	 * @details Sets up the UI and enriches the displayed text with
	 * compilation information including:
	 * - Full version string
	 * - Compiler (MSVC, GCC, Clang)
	 * - Qt version
	 */
	ccAboutDialog(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 *
	 * @details Cleans up the UI structure.
	 */
	~ccAboutDialog();

  private:
	/**
	 * @brief UI definition.
	 *
	 * Auto-generated from the .ui file.
	 */
	Ui::AboutDialog* mUI;
};

#endif // CCABOUTDIALOG_H
