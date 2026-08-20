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
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        //
// ##########################################################################

/**
 * @file ccSetSFAsVec3Dlg.h
 *
 * @brief Dialog for mapping scalar fields to Vec3 components.
 *
 * @details Dialog for assigning scalar fields to the X, Y, Z components
 * of a 3D vector.
 *
 * Used for:
 * - Creating normal vectors from SFs
 * - Creating RGB color components from SFs
 * - Creating displacement vectors from SFs
 *
 * Special options:
 * - Use a constant (0 or 1) instead of SF
 * - Keep existing value unchanged
 *
 * @author CloudCompare project
 */

#include <ui_setSFAsVec3Dlg.h>

// Qt
#include <QDialog>

class ccPointCloud;

/**
 * @brief Dialog for mapping scalar fields to Vec3 components.
 *
 * @details Allows selecting up to 3 scalar fields to represent
 * the X, Y, Z components of a vector.
 *
 * Common uses:
 * - Normal vectors from scalar field gradients
 * - Color normalization
 * - Coordinate transformation
 *
 * @extends QDialog
 */
class ccSetSFsAsVec3Dialog : public QDialog
    , public Ui::SetSFsAsVec3Dialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Special scalar field index: no SF selected.
	 */
	static const int SF_INDEX_NO = -1;

	/**
	 * @brief Special scalar field index: use zero.
	 */
	static const int SF_INDEX_ZERO = -2;

	/**
	 * @brief Special scalar field index: use one.
	 */
	static const int SF_INDEX_ONE = -3;

	/**
	 * @brief Special scalar field index: keep unchanged.
	 */
	static const int SF_INDEX_UNCHANGED = -4;

	/**
	 * @brief Construct the dialog.
	 *
	 * @param[in] cloud Point cloud with SFs.
	 * @param[in] xLabel Label for X component.
	 * @param[in] yLabel Label for Y component.
	 * @param[in] zLabel Label for Z component.
	 * @param[in] allowUnchanged Show unchanged option.
	 * @param[in] parent Parent widget.
	 */
	ccSetSFsAsVec3Dialog(const ccPointCloud* cloud,
	                     const QString&      xLabel,
	                     const QString&      yLabel,
	                     const QString&      zLabel,
	                     bool                allowUnchanged,
	                     QWidget*            parent = nullptr);

	/**
	 * @brief Set initial SF indexes.
	 *
	 * @param[in] sf1Index X component SF index.
	 * @param[in] sf2Index Y component SF index.
	 * @param[in] sf3Index Z component SF index.
	 */
	void setSFIndexes(int sf1Index, int sf2Index, int sf3Index);

	/**
	 * @brief Get selected SF indexes.
	 *
	 * @param[out] sf1Index X component SF index.
	 * @param[out] sf2Index Y component SF index.
	 * @param[out] sf3Index Z component SF index.
	 */
	void getSFIndexes(int& sf1Index, int& sf2Index, int& sf3Index) const;

  protected:
	/**
	 * @brief Convert SF index to combo box index.
	 *
	 * @param[in] index SF index.
	 * @return Combo box index.
	 */
	int toComboBoxIndex(int index) const;

	/**
	 * @brief Convert combo box index to SF index.
	 *
	 * @param[in] index Combo box index.
	 * @return SF index.
	 */
	int fromComboBoxIndex(int index) const;

	//! Number of constant field options
	int m_constFields;
};
