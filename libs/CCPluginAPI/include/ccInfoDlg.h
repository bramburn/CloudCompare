// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        //
// ##########################################################################

/**
 * @file ccInfoDlg.h
 *
 * @brief Simple information dialog for displaying messages.
 *
 * @details Provides a lightweight dialog for showing informational
 * text to users during processing operations.
 *
 * ## Usage
 *
 * @code
 * ccInfoDlg info(this);
 * info.showText("Processing complete.\n\n3,456 points processed.");
 * info.exec();
 * @endcode
 *
 * ## Comparison with Other Dialogs
 *
 * - **ccConsole**: Persistent log window
 * - **QMessageBox**: Standard Qt dialog with icons
 * - **ccInfoDlg**: Lightweight inline info display
 *
 * @author CloudCompare project
 */

#include "CCPluginAPI.h"

// Qt
#include <QDialog>

namespace Ui
{
	class InfoDialog;
}

/**
 * @brief Simple information dialog.
 *
 * @details Lightweight dialog for displaying text information.
 *
 * Features:
 * - Plain text display
 * - Auto-wrapped text
 * - Modal behavior
 *
 * @extends QDialog
 */
class CCPLUGIN_LIB_API ccInfoDlg : public QDialog
{
  public:
	/**
	 * @brief Construct the info dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	ccInfoDlg(QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	~ccInfoDlg() override;

	/**
	 * @brief Set the display text.
	 *
	 * @param[in] text Text to display.
	 *
	 * @note Supports newline characters for multi-line text.
	 */
	void showText(const QString& text);

  private:
	//! UI definition.
	Ui::InfoDialog* m_ui;
};
