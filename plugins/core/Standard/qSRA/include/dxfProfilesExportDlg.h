//##########################################################################
//#                                                                        #
//#                      CLOUDCOMPARE PLUGIN: qSRA                         #
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
//#                           COPYRIGHT: EDF                               #
//#                                                                        #
//##########################################################################

#ifndef QSRA_DXF_PROFILE_IMPORT_DLG_HEADER
#define QSRA_DXF_PROFILE_IMPORT_DLG_HEADER

/**
 * @file dxfProfilesExportDlg.h
 *
 * @brief DXF profiles export dialog
 *
 * Dialog for exporting profiles to DXF format.
 */

#include "ui_dxfProfilesExportDlg.h"

/**
 * @class DxfProfilesExportDlg
 *
 * @brief DXF profiles export dialog
 *
 * Export profiles to DXF format.
 */
class DxfProfilesExportDlg : public QDialog, public Ui::DxfProfilesExportDlg
{
	Q_OBJECT

public:

	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit DxfProfilesExportDlg(QWidget* parent = nullptr);

	/**
	 * @brief Get vertical profiles filename
	 * @return Output filename
	 */
	QString getVertFilename() const;
	
	/**
	 * @brief Get horizontal profiles filename
	 * @return Output filename
	 */
	QString getHorizFilename() const;

protected:
	/// Browse vertical file
	void browseVertFile();
	/// Browse horizontal file
	void browseHorizFile();

	/// Accept and save settings
	void acceptAndSaveSettings();

protected:
	/// Initialize from settings
	void initFromPersistentSettings();

};

#endif //QSRA_PROFILE_IMPORT_DLG_HEADER
