#pragma once

//##########################################################################
//#                                                                        #
//#                     CLOUDCOMPARE PLUGIN: qFacets                       #
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
//#                      COPYRIGHT: Thomas Dewez, BRGM                     #
//#                                                                        #
//##########################################################################

/**
 * @file facetsExportDlg.h
 *
 * @brief Facets export dialog
 *
 * Dialog for exporting facets.
 */

#include <QDialog>

#include "ui_facetsExportDlg.h"

/**
 * @class FacetsExportDlg
 *
 * @brief Facets export dialog
 *
 * Dialog for exporting facets or facets info.
 */
class FacetsExportDlg : public QDialog, public Ui::FacetsExportDlg
{
	Q_OBJECT

public:

	/**
	 * @enum IOMode
	 *
	 * @brief I/O mode
	 */
	enum IOMode { SHAPE_FILE_IO, ASCII_FILE_IO };

	/**
	 * @brief Create dialog
	 * @param[in] mode I/O mode
	 * @param[in] parent Parent widget
	 */
	FacetsExportDlg(IOMode mode, QWidget* parent = nullptr);

protected:
	/// Browse for destination
	void browseDestination();

protected:
	/// Current I/O mode
	IOMode m_mode;
};
