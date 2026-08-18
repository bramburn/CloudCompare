#pragma once
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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

/**
 * @file ccInfoDlg.h
 *
 * @brief Information dialog widget
 *
 * Simple dialog for displaying informational messages.
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
 * @brief Information dialog
 *
 * Simple dialog for showing text information.
 */
class CCPLUGIN_LIB_API ccInfoDlg : public QDialog
{
  public:
	/**
	 * @brief Create info dialog
	 * @param[in] parent Parent widget
	 */
	ccInfoDlg(QWidget* parent);

	/**
	 * @brief Destructor
	 */
	~ccInfoDlg() override;

	/**
	 * @brief Show text in dialog
	 * @param[in] text Text to display
	 */
	void showText(const QString& text);

  private:
	/// UI
	Ui::InfoDialog* m_ui;
};
