//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qHPR                        #
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
//#                  COPYRIGHT: Daniel Girardeau-Montaut                   #
//#                                                                        #
//##########################################################################

#ifndef CC_HPR_DLG_HEADER
#define CC_HPR_DLG_HEADER

/**
 * @file ccHprDlg.h
 *
 * @brief HPR dialog
 *
 * Dialog for Hidden Point Removal parameters.
 */

#include "ui_hprDlg.h"

/**
 * @class ccHprDlg
 *
 * @brief HPR dialog
 *
 * Dialog for HPR parameters.
 */
class ccHprDlg : public QDialog, public Ui::HPRDialog
{
public:

	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccHprDlg(QWidget* parent = nullptr);
};

#endif
