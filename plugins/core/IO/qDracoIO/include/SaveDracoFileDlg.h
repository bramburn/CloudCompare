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
 * @file SaveDracoFileDlg.h
 *
 * @brief Save Draco file dialog
 *
 * Dialog for configuring Draco file export options.
 *
 * @author CloudCompare project
 */

#include "ui_saveDracoFileDlg.h"

#include <QDialog>
#include <qCC_io.h>

/**
 * @class SaveDracoFileDlg
 *
 * @brief Save Draco file dialog
 *
 * Configure Draco file export options.
 */
class SaveDracoFileDlg : public QDialog
    , public Ui::SaveDracoFileDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit SaveDracoFileDlg(QWidget* parent = nullptr);

	/// Destructor
	virtual ~SaveDracoFileDlg() = default;

	/// Reset to defaults
	void reset();

  protected:
	/// Initialize from settings
	void initFromPersistentSettings();

	/// Save to settings
	void saveToPersistentSettings();
};
