//##########################################################################
//#                                                                        #
//#                    CLOUDCOMPARE PLUGIN: ccCompass                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                     COPYRIGHT: Sam Thiele  2017                        #
//#                                                                        #
//##########################################################################

#ifndef CC_MAP_DIALOG_HEADER
#define CC_MAP_DIALOG_HEADER

/**
 * @file ccMapDlg.h
 *
 * @brief Map dialog
 *
 * Map mode overlay dialog for compass.
 */

#include <QDialog>
#include <QList>
#include <QAction>

#include <ccOverlayDialog.h>

#include <ui_mapDlg.h>
#include "ccTrace.h"

/**
 * @class ccMapDlg
 *
 * @brief Map dialog
 *
 * Map mode overlay dialog for creating GeoObjects.
 */
class ccMapDlg : public ccOverlayDialog, public Ui::mapDlg
{
	Q_OBJECT

public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccMapDlg(QWidget* parent = nullptr);

	/// Create object menu
	QMenu *m_createObject_menu;

	/// Create GeoObject action
	QAction *m_create_geoObject;
	
	/// Create single surface GeoObject action
	QAction *m_create_geoObjectSS;
};

#endif
