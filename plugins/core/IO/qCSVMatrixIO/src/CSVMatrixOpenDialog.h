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

#ifndef CSV_MATRIX_OPEN_DIALOG_HEADER
#define CSV_MATRIX_OPEN_DIALOG_HEADER

/**
 * @file CSVMatrixOpenDialog.h
 *
 * @brief CSV Matrix open dialog
 *
 * Dialog for opening CSV matrix files.
 *
 * @author Daniel Girardeau-Montaut
 */

// Qt
#include <QDialog>

// GUI
#include "ui_openCSVMatrixDlg.h"

/**
 * @brief CSV Matrix open dialog
 *
 * Configure CSV matrix file import options.
 */
class CSVMatrixOpenDialog : public QDialog
    , public Ui::CSVMatrixOpenDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit CSVMatrixOpenDialog(QWidget* parent = 0);

  protected:
	/// Browse for texture file
	void browseTextureFile();
};

#endif
