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
// #          COPYRIGHT: Chris Brown                                        #
// #                                                                        #
// ##########################################################################

/**
 * @file ccPrimitiveDistanceDlg.h
 *
 * @brief Primitive distance dialog
 *
 * Dialog for cloud-to-primitive distance settings.
 *
 * @author Chris Brown
 */

// Qt
#include <QDialog>
#include <ui_primitiveDistanceDlg.h>

/**
 * @brief Primitive distance dialog
 *
 * Configure cloud-to-primitive distance settings.
 */
class ccPrimitiveDistanceDlg : public QDialog
    , public Ui::primitiveDistanceDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	ccPrimitiveDistanceDlg(QWidget* parent = nullptr);

	/// Get signed distances flag
	inline bool signedDistances() const
	{
		return signedDistCheckBox->isChecked();
	}
	
	/// Get flip normals flag
	inline bool flipNormals() const
	{
		return flipNormalsCheckBox->isChecked();
	}
	
	/// Get treat planes as bounded flag
	inline bool treatPlanesAsBounded() const
	{
		return treatPlanesAsBoundedCheckBox->isChecked();
	}
};
