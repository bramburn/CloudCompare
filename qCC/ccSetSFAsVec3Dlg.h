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
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

/**
 * @file ccSetSFAsVec3Dlg.h
 *
 * @brief Set scalar fields as Vec3 dialog
 *
 * Dialog for mapping scalar fields to Vec3 components.
 *
 * @author CloudCompare project
 */

#include <ui_setSFAsVec3Dlg.h>

// Qt
#include <QDialog>

class ccPointCloud;

/**
 * @brief Set scalar fields as Vec3 dialog
 *
 * Choose up to 3 scalar fields to use as Normal components.
 */
class ccSetSFsAsVec3Dialog : public QDialog
    , public Ui::SetSFsAsVec3Dialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] cloud Point cloud
	 * @param[in] xLabel Label for X component
	 * @param[in] yLabel Label for Y component
	 * @param[in] zLabel Label for Z component
	 * @param[in] allowUnchanged Allow unchanged option
	 * @param[in] parent Parent widget
	 */
	ccSetSFsAsVec3Dialog(const ccPointCloud* cloud,
	                     const QString&      xLabel,
	                     const QString&      yLabel,
	                     const QString&      zLabel,
	                     bool                allowUnchanged,
	                     QWidget*            parent = nullptr);

	/// No SF index
	static const int SF_INDEX_NO = -1;
	/// Zero SF index
	static const int SF_INDEX_ZERO = -2;
	/// One SF index
	static const int SF_INDEX_ONE = -3;
	/// Unchanged index
	static const int SF_INDEX_UNCHANGED = -4;

	/// Set SF indexes
	void setSFIndexes(int sf1Index, int sf2Index, int sf3Index);

	/// Get SF indexes
	void getSFIndexes(int& sf1Index, int& sf2Index, int& sf3Index) const;

  protected:
	/// Convert SF index to combo box index
	int toComboBoxIndex(int index) const;

	/// Convert combo box index to SF index
	int fromComboBoxIndex(int index) const;

	/// Number of constant fields at beginning
	int m_constFields;
};
