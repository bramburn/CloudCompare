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

#ifndef CC_MATRIX_DISPLAY_DIALOG_HEADER
#define CC_MATRIX_DISPLAY_DIALOG_HEADER

/**
 * @file matrixDisplayDlg.h
 *
 * @brief Matrix display dialog
 *
 * Widget for displaying 4x4 transformation matrices.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QWidget>

// qCC_db
#include <ccGLMatrix.h>

namespace Ui
{
	class MatrixDisplayDlg;
}

/**
 * @brief Matrix display dialog
 *
 * Display 4x4 matrices in various formats.
 */
class MatrixDisplayDlg : public QWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Create widget
	 * @param[in] parent Parent widget
	 */
	explicit MatrixDisplayDlg(QWidget* parent = nullptr);

	/// Destructor
	~MatrixDisplayDlg();

	/// Clear widget
	void clear();

	/**
	 * @brief Fill with float matrix
	 * @param[in] mat Matrix
	 */
	void fillDialogWith(const ccGLMatrix& mat);

	/**
	 * @brief Fill with double matrix
	 * @param[in] mat Matrix
	 */
	void fillDialogWith(const ccGLMatrixd& mat);

  public:
	/// Export to ASCII file
	void exportToASCII();

	/// Export to clipboard
	void exportToClipboard();

  protected:
	/// Fill with axis-angle and translation
	void fillDialogWith(const CCVector3d& axis, double angle_rad, const CCVector3d& T, int precision);

	/// Matrix
	ccGLMatrixd m_mat;

	Ui::MatrixDisplayDlg* m_ui;
};

#endif // CC_MATRIX_DISPLAY_DIALOG_HEADER
