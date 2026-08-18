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

#ifndef QSRA_PROFILE_IMPORT_DLG_HEADER
#define QSRA_PROFILE_IMPORT_DLG_HEADER

/**
 * @file profileImportDlg.h
 *
 * @brief Profile import dialog
 *
 * Dialog for importing 2D revolution profiles.
 */

#include "ui_profileImportDlg.h"

/**
 * @class ProfileImportDlg
 *
 * @brief Profile import dialog
 *
 * Dialog for importing 2D revolution profiles.
 */
class ProfileImportDlg : public QDialog, public Ui::ProfileImportDlg
{
	Q_OBJECT

public:

	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
    explicit ProfileImportDlg(QWidget* parent = nullptr);

	/**
	 * @brief Get axis dimension
	 * @return 0(X), 1(Y) or 2(Z)
	 */
	int getAxisDimension() const;

	/**
	 * @brief Set default filename
	 * @param[in] filename Default filename
	 */
	void setDefaultFilename(QString filename);

	/**
	 * @brief Get filename
	 * @return Input filename
	 */
	QString getFilename() const;

	/**
	 * @brief Check if heights are absolute
	 * @return True if heights are absolute
	 */
	bool absoluteHeightValues() const;

protected:
	/// Browse for file
	void browseFile();

};

#endif //QSRA_PROFILE_IMPORT_DLG_HEADER
