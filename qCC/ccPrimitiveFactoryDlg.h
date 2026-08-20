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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_PRIMITIVE_FACTORY_DLG_HEADER
#define CC_PRIMITIVE_FACTORY_DLG_HEADER

/**
 * @file ccPrimitiveFactoryDlg.h
 *
 * @brief Primitive factory dialog
 *
 * Dialog for creating primitive shapes.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <ui_primitiveFactoryDlg.h>

// Qt
#include <QDialog>

class MainWindow;
class ccGLMatrix;

/**
 * @brief Primitive factory dialog
 *
 * Create primitive shapes.
 */
class ccPrimitiveFactoryDlg : public QDialog
    , public Ui::PrimitiveFactoryDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] win Main window
	 */
	explicit ccPrimitiveFactoryDlg(MainWindow* win);

  protected:
	/// Create primitive
	void createPrimitive();

	/// Set sphere position from clipboard
	void setSpherePositionFromClipboard();

	/// Set sphere position to origin
	void setSpherePositionToOrigin();

	/// Set coordinate system based on selected object
	void setCoordinateSystemBasedOnSelectedObject();

	/// Handle matrix text change
	void onMatrixTextChange();

	/// Set CS matrix to identity
	void setCSMatrixToIdentity();

	/// Get CS matrix
	ccGLMatrix getCSMatrix(bool& valid);

  protected:
	/// Main window
	MainWindow* m_win;
};

#endif // CC_PRIMITIVE_FACTORY_DLG_HEADER
