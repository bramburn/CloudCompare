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
 * @brief Primitive factory dialog for creating geometric primitives.
 *
 * @details Dialog for creating 3D geometric primitive shapes such as:
 * - Sphere
 * - Plane
 * - Box/Cube
 * - Cylinder
 * - Cone
 * - Torus
 * - etc.
 *
 * Each primitive can be positioned and oriented in 3D space
 * using coordinate system settings.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPrimitiveFactory
 */

#include <ui_primitiveFactoryDlg.h>

// Qt
#include <QDialog>

class MainWindow;
class ccGLMatrix;

/**
 * @brief Dialog for creating primitive shapes.
 *
 * @details Provides a UI for generating 3D geometric primitives
 * with configurable parameters and coordinate system.
 *
 * Features:
 * - Primitive type selection (sphere, plane, box, etc.)
 * - Size/dimension parameters
 * - Position (X, Y, Z coordinates)
 * - Orientation (via coordinate system matrix)
 * - Import position from clipboard
 * - Use selected object as coordinate system reference
 *
 * @extends QDialog
 * @extends Ui::PrimitiveFactoryDlg
 */
class ccPrimitiveFactoryDlg : public QDialog
    , public Ui::PrimitiveFactoryDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the primitive factory dialog.
	 *
	 * @param[in] win Main window for interaction.
	 */
	explicit ccPrimitiveFactoryDlg(MainWindow* win);

  protected slots:
	/**
	 * @brief Create the selected primitive.
	 */
	void createPrimitive();

	/**
	 * @brief Set sphere position from clipboard.
	 */
	void setSpherePositionFromClipboard();

	/**
	 * @brief Set sphere position to origin.
	 */
	void setSpherePositionToOrigin();

	/**
	 * @brief Set coordinate system from selected object.
	 */
	void setCoordinateSystemBasedOnSelectedObject();

	/**
	 * @brief Handle matrix text change.
	 */
	void onMatrixTextChange();

	/**
	 * @brief Reset coordinate system to identity.
	 */
	void setCSMatrixToIdentity();

	/**
	 * @brief Get the coordinate system matrix.
	 *
	 * @param[out] valid Whether the matrix is valid.
	 * @return The coordinate system transformation matrix.
	 */
	ccGLMatrix getCSMatrix(bool& valid);

  protected:
	//! Main window
	MainWindow* m_win;
};

#endif // CC_PRIMITIVE_FACTORY_DLG_HEADER
