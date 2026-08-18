//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qM3C2                       #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#            COPYRIGHT: UNIVERSITE EUROPEENNE DE BRETAGNE                #
//#                                                                        #
//##########################################################################

#ifndef M3C2_DISCLAIMER_DIALOG_HEADER
#define M3C2_DISCLAIMER_DIALOG_HEADER

/**
 * @file qM3C2DisclaimerDialog.h
 *
 * @brief M3C2 disclaimer dialog
 *
 * Disclaimer dialog for M3C2 plugin.
 */

#include <QDialog>

class ccMainAppInterface;
 
namespace Ui {
	class DisclaimerDialog;
}

/**
 * @class DisclaimerDialog
 *
 * @brief M3C2 disclaimer dialog
 *
 * Display plugin disclaimer to user.
 */
class DisclaimerDialog : public QDialog
{
public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	DisclaimerDialog(QWidget* parent = nullptr);
	
	/// Destructor
	~DisclaimerDialog();
	
	/**
	 * @brief Show disclaimer
	 * @param[in] app Main application interface
	 * @return Whether disclaimer was accepted
	 */
	static bool show(ccMainAppInterface* app);
	
private:
	/// Disclaimer accepted flag
	static bool s_disclaimerAccepted;
	
	/// UI
	Ui::DisclaimerDialog* m_ui;
};

#endif //M3C2_DISCLAIMER_DIALOG_HEADER
