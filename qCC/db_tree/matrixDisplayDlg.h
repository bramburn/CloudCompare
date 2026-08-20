// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        //
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file matrixDisplayDlg.h
 *
 * @brief 4x4 transformation matrix display widget.
 *
 * @details Widget for displaying 4x4 transformation matrices
 * used throughout CloudCompare for geometric transformations.
 *
 * Displays:
 * - Standard 4x4 matrix format
 * - Axis-angle + translation decomposition
 * - Euler angles (computed)
 *
 * Supports:
 * - Float and double precision
 * - Export to ASCII file
 * - Export to clipboard
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGLMatrix for matrix implementation
 */

#ifndef CC_MATRIX_DISPLAY_DIALOG_HEADER
#define CC_MATRIX_DISPLAY_DIALOG_HEADER

// Qt
#include <QWidget>

// qCC_db
#include <ccGLMatrix.h>

namespace Ui
{
	class MatrixDisplayDlg;
}

/**
 * @brief Widget for displaying transformation matrices.
 *
 * @details Provides a visual representation of 4x4 transformation
 * matrices with multiple display formats.
 *
 * Features:
 * - 4x4 matrix view
 * - Axis-angle + translation
 * - Precision control
 * - Export options
 *
 * @extends QWidget
 */
class MatrixDisplayDlg : public QWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the matrix display widget.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit MatrixDisplayDlg(QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~MatrixDisplayDlg();

	/**
	 * @brief Clear the display.
	 */
	void clear();

	/**
	 * @brief Fill with float matrix.
	 *
	 * @param[in] mat Transformation matrix.
	 */
	void fillDialogWith(const ccGLMatrix& mat);

	/**
	 * @brief Fill with double matrix.
	 *
	 * @param[in] mat Transformation matrix.
	 */
	void fillDialogWith(const ccGLMatrixd& mat);

  public slots:
	/**
	 * @brief Export matrix to ASCII file.
	 */
	void exportToASCII();

	/**
	 * @brief Export matrix to clipboard.
	 */
	void exportToClipboard();

  protected:
	/**
	 * @brief Fill with axis-angle and translation.
	 *
	 * @param[in] axis Rotation axis.
	 * @param[in] angle_rad Angle in radians.
	 * @param[in] T Translation vector.
	 * @param[in] precision Decimal precision.
	 */
	void fillDialogWith(const CCVector3d& axis, double angle_rad, const CCVector3d& T, int precision);

  private:
	//! Stored matrix
	ccGLMatrixd m_mat;

	//! UI definition
	Ui::MatrixDisplayDlg* m_ui;
};

#endif // CC_MATRIX_DISPLAY_DIALOG_HEADER
