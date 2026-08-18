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

#ifndef CC_COMPASS_INFO_HEADER
#define CC_COMPASS_INFO_HEADER

/**
 * @file ccCompassInfo.h
 *
 * @brief Compass info dialog
 *
 * Help dialog for the Compass plugin.
 */

#include <QDialog>
#include <QDialogButtonBox>

/**
 * @class ccCompassInfo
 *
 * @brief Compass info dialog
 *
 * Display help information for the Compass plugin.
 */
class ccCompassInfo : public QDialog
{
	Q_OBJECT
	
public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccCompassInfo(QWidget *parent = nullptr);
};

#endif
