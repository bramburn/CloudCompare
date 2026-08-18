//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qBroom                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#      COPYRIGHT: Wesley Grimes (Collision Engineering Associates)       #
//#                                                                        #
//##########################################################################

#ifndef QBROOM_DISCLAIMER_DIALOG_HEADER
#define QBROOM_DISCLAIMER_DIALOG_HEADER

/**
 * @file qBroomDisclaimerDialog.h
 *
 * @brief Disclaimer dialog
 *
 * Disclaimer dialog for the broom plugin.
 */

#include <ui_disclaimerDlg.h>

//qCC_plugins
#include <ccMainAppInterface.h>

//Qt
#include <QMainWindow>
#include <QDialog>

/**
 * @class DisclaimerDialog
 *
 * @brief Disclaimer dialog
 *
 * Display plugin disclaimer to user.
 */
class DisclaimerDialog : public QDialog, public Ui::DisclaimerDialog
{
public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	DisclaimerDialog(QWidget* parent = nullptr)
		: QDialog(parent)
		, Ui::DisclaimerDialog()
	{
		setupUi(this);
	}
};

/// Disclaimer accepted flag
static bool s_disclaimerAccepted = false;

/**
 * @brief Show disclaimer
 * @param[in] app Main application interface
 * @return Whether disclaimer was accepted
 */
static bool ShowDisclaimer(ccMainAppInterface* app)
{
	if (!s_disclaimerAccepted)
	{
		s_disclaimerAccepted = DisclaimerDialog(app ? app->getMainWindow() : 0).exec();
	}
	
	return s_disclaimerAccepted;
}

#endif //QBROOM_DISCLAIMER_DIALOG_HEADER
