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

#ifndef CC_APPLY_TRANSFORMATION_DLG_HEADER
#define CC_APPLY_TRANSFORMATION_DLG_HEADER

/**
 * @file ccApplyTransformationDlg.h
 *
 * @brief Apply transformation dialog for entering transformation matrices.
 *
 * @details Dialog for manually entering 4x4 transformation matrices
 * to apply to entities. Supports multiple input formats:
 * - Direct matrix entry (16 values)
 * - Axis-angle representation
 * - Euler angles
 * - From/to axis definition
 *
 * The dialog validates the matrix and allows loading from
 * ASCII files or clipboard.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGLMatrixd
 * @see ccGraphicalTransformationTool
 */

#include <ui_applyTransformationDlg.h>

// qCC_db
#include <ccGLMatrix.h>

/**
 * @brief Dialog for entering transformation matrices.
 *
 * @details Provides multiple ways to define a transformation:
 *
 * 1. **Direct matrix entry**: Enter all 16 values of the 4x4 matrix
 *
 * 2. **Axis-angle**: Specify rotation axis and angle
 *    - Axis: X, Y, or Z (or arbitrary direction)
 *    - Angle: degrees or radians
 *
 * 3. **Euler angles**: Specify rotations around X, Y, Z axes
 *    - Order: Rx, Ry, Rz
 *    - Can specify translation separately
 *
 * 4. **From/to axes**: Define transformation by source and
 *    destination axis systems
 *
 * Features:
 * - Real-time matrix preview
 * - Load from file or clipboard
 * - Initialize from dip/dip direction (geological convention)
 * - Validation of matrix properties
 *
 * @extends QDialog
 * @extends Ui::ApplyTransformationDialog
 */
class ccApplyTransformationDlg : public QDialog
    , public Ui::ApplyTransformationDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the transformation dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccApplyTransformationDlg(QWidget* parent = nullptr);

	/**
	 * @brief Get the entered transformation matrix.
	 *
	 * @param[out] applyToGlobal Whether to apply to global shift.
	 * @return The 4x4 transformation matrix.
	 *
	 * @details Returns the transformation matrix configured by the user.
	 * The applyToGlobal flag indicates whether the transformation
	 * should also affect the entity's global shift.
	 */
	ccGLMatrixd getTransformation(bool& applyToGlobal) const;

  protected slots:
	/**
	 * @brief Check matrix validity and accept dialog.
	 *
	 * @details Validates the entered matrix and closes the dialog
	 * if valid. Shows an error message if the matrix is invalid.
	 */
	void checkMatrixValidityAndAccept();

	/**
	 * @brief Handle matrix text change.
	 *
	 * @details Called when the user edits the matrix text field.
	 * Updates all other forms to reflect the new values.
	 */
	void onMatrixTextChange();

	/**
	 * @brief Handle axis-angle form changes.
	 *
	 * @param[in] value Changed value.
	 *
	 * @details Updates the matrix when rotation axis or angle changes.
	 */
	void onRotAngleValueChanged(double value);

	/**
	 * @brief Handle Euler angles form changes.
	 *
	 * @param[in] value Changed value.
	 *
	 * @details Updates the matrix when any Euler angle changes.
	 */
	void onEulerValueChanged(double value);

	/**
	 * @brief Handle from/to axis form changes.
	 *
	 * @param[in] value Changed value.
	 *
	 * @details Updates the matrix when from/to axes change.
	 */
	void onFromToValueChanged(double value);

	/**
	 @brief Load matrix from ASCII file.
	 */
	void loadFromASCIIFile();

	/**
	 * @brief Load matrix from clipboard.
	 */
	void loadFromClipboard();

	/**
	 * @brief Initialize from dip and dip direction.
	 *
	 * @details Computes a rotation matrix from geological
	 * dip and dip direction angles.
	 */
	void initFromDipAndDipDir();

	/**
	 * @brief Handle button clicks.
	 *
	 * @param[in] button Clicked button.
	 */
	void buttonClicked(QAbstractButton* button);

	/**
	 * @brief Load rotation axis from clipboard.
	 */
	void axisFromClipboard();

	/**
	 * @brief Load translation from clipboard.
	 */
	void transFromClipboard();

	/**
	 * @brief Load Euler angles from clipboard.
	 */
	void eulerAnglesFromClipboard();

	/**
	 * @brief Load Euler translation from clipboard.
	 */
	void eulerTransFromClipboard();

	/**
	 * @brief Load "from" axis from clipboard.
	 */
	void fromAxisFromClipboard();

	/**
	 * @brief Load "to" axis from clipboard.
	 */
	void toAxisFromClipboard();

	/**
	 * @brief Load from/to translation from clipboard.
	 */
	void fromToTransFromClipboard();

  protected:
	/**
	 * @brief Update all forms with a matrix.
	 *
	 * @param[in] mat Transformation matrix.
	 * @param[in] textForm Update text form.
	 * @param[in] axisAngleForm Update axis-angle form.
	 * @param[in] eulerForm Update Euler form.
	 * @param[in] fromToForm Update from/to form.
	 *
	 * @details Synchronizes all input forms to show the same matrix.
	 */
	void updateAll(const ccGLMatrixd& mat, bool textForm = true, bool axisAngleForm = true, bool eulerForm = true, bool fromToForm = true);
};

#endif // CC_APPLY_TRANSFORMATION_DLG_HEADER
