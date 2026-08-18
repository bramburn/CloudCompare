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

#ifndef CCCOMPASSEXPORT_H
#define CCCOMPASSEXPORT_H

/**
 * @file ccCompassExport.h
 *
 * @brief Compass export functions
 *
 * Export compass measurements to various formats.
 */

#include <QString>

class ccMainAppInterface;

/**
 * @class ccCompassExport
 *
 * @brief Compass export
 *
 * Export geological measurements to various formats.
 */
class ccCompassExport
{
public:
	/**
	 * @brief Save to CSV
	 * @param[in] app Main application interface
	 * @param[in] filename Output filename
	 */
	static void SaveCSV(ccMainAppInterface* app, const QString& filename);
	
	/**
	 * @brief Save to SVG
	 * @param[in] app Main application interface
	 * @param[in] filename Output filename
	 * @param[in] zoom Zoom level
	 */
	static void SaveSVG(ccMainAppInterface* app, const QString& filename, float zoom);
	
	/**
	 * @brief Save to XML
	 * @param[in] app Main application interface
	 * @param[in] filename Output filename
	 */
	static void SaveXML(ccMainAppInterface* app, const QString& filename);
};

#endif // CCCOMPASSEXPORT_H
