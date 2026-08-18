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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccAboutDialog.h
 *
 * @brief About dialog
 *
 * Dialog showing application information.
 *
 * @author CloudCompare project
 */

#include <QDialog>

namespace Ui
{
	class AboutDialog;
}

/**
 * @brief About dialog
 *
 * Shows application version and credits.
 */
class ccAboutDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	ccAboutDialog(QWidget* parent = nullptr);
	
	/**
	 * @brief Destructor
	 */
	~ccAboutDialog();

  private:
	/// UI
	Ui::AboutDialog* mUI;
};

#endif // CCABOUTDIALOG_H
