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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccScalarFieldsManagerDlg.h
 *
 * @brief Scalar fields manager dialog for editing and creating SFs.
 *
 * @details Dialog for managing scalar fields attached to point clouds.
 *
 * Scalar fields store per-point numerical values used for:
 * - Color mapping (heights, intensities, etc.)
 * - Classification labels
 * - Quality metrics
 * - Measurement results
 *
 * Features:
 * - View SF statistics (min, max, mean, std dev)
 * - Rename scalar fields
 * - Delete scalar fields
 * - Add constant scalar fields
 * - Show SF histogram
 * - Scalar field arithmetic
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

// qCC_db
#include <ccHObject.h>
#include <ccScalarField.h>

class ccScalarField;
class ccPointCloud;

namespace Ui
{
	class ScalarFieldsManagerDlg;
}

/**
 * @brief Dialog for managing scalar fields.
 *
 * @details Provides a UI for viewing and editing scalar fields.
 *
 * Features:
 * - Display SF statistics table
 * - Rename, delete, add SFs
 * - View histogram
 * - Arithmetic operations
 *
 * @extends QDialog
 */
class ccScalarFieldsManagerDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Scalar field display attributes.
	 */
	enum SFAttributes
	{
		NAME   = 0, //!< SF name
		MINVAL = 1, //!< Minimum value
		MAXVAL = 2, //!< Maximum value
		MEAN   = 3, //!< Mean value
		STD    = 4  //!< Standard deviation
	};

	/**
	 * @brief Construct the scalar fields manager.
	 *
	 * @param[in] selectedEntities Entities to manage SFs for.
	 * @param[in] parent Parent widget.
	 */
	ccScalarFieldsManagerDialog(const ccHObject::Container& selectedEntities,
	                            QWidget*                    parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccScalarFieldsManagerDialog() override;

	/**
	 * @brief Set the active point cloud.
	 *
	 * @param[in] pc Point cloud.
	 */
	void setActivePointCloud(ccPointCloud* pc);

  protected slots:
	/**
	 * @brief Set selected entities.
	 *
	 * @param[in] entities Selected entities.
	 */
	void setSelectedEntities(const ccHObject::Container& entities);

	/**
	 * @brief Handle entity selection change.
	 *
	 * @param[in] index New selection index.
	 */
	void onEntityChanged(int index);

	/**
	 * @brief Build the statistics table.
	 */
	void buildTable();

	/**
	 * @brief Update display.
	 */
	void updateDisplay();

	/**
	 * @brief Delete selected SF.
	 */
	void deleteSF();

	/**
	 * @brief Rename a scalar field.
	 *
	 * @param[in] sfIdx SF index.
	 * @param[in] newName New name.
	 */
	void renameSF(int sfIdx, const QString& newName);

	/**
	 * @brief Add a constant scalar field.
	 */
	void addConstantSF();

	/**
	 * @brief Show SF histogram.
	 */
	void showHistogram();

	/**
	 * @brief Open arithmetic dialog.
	 */
	void doArithmetic();

	/**
	 * @brief Append SF to table.
	 *
	 * @param[in] sfIdx SF index.
	 */
	void appendSFToTable(int sfIdx);

  protected:
	//! Active point cloud
	ccPointCloud* m_pointCloud;

	//! SF count
	unsigned m_sfCount;

	//! Available point clouds
	std::vector<ccPointCloud*> m_availableClouds;

	//! UI definition
	Ui::ScalarFieldsManagerDlg* m_ui;
};
