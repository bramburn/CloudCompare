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
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccColorScaleEditorDlg.h
 *
 * @brief Dialog for creating and editing color scales.
 *
 * @details Modal dialog for creating, editing, and managing color scales
 * used in scalar field visualization.
 *
 * ## Features
 *
 * - Create new color scales
 * - Edit existing scales
 * - Relative vs. absolute mode
 * - Custom labels
 * - Import/export scales
 * - Delete scales
 *
 * ## Scale Modes
 *
 * ### Relative Mode
 * Colors are distributed evenly across the [0,1] range.
 * Good for normalized data.
 *
 * ### Absolute Mode
 * Colors map to actual scalar values.
 * Good for data with known min/max bounds.
 *
 * ## Usage
 *
 * @code
 * ccColorScaleEditorDialog dialog(manager, app, currentScale, this);
 * if (dialog.exec() == QDialog::Accepted) {
 *     ccColorScale::Shared scale = dialog.getActiveScale();
 *     // Use the scale
 * }
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccColorScaleEditorWidget for the editor widget
 * @see ccColorScalesManager for scale management
 */

#pragma once

#include "CCPluginAPI.h"

// qCC_db
#include <ccColorScale.h>

// Qt
#include <QDialog>

class ccScalarField;
class ccColorScaleEditorWidget;
class ccColorScalesManager;
class ccMainAppInterface;

namespace Ui
{
	class ColorScaleEditorDlg;
}

/**
 * @brief Dialog for color scale creation and editing.
 *
 * @details Provides a complete interface for managing color scales.
 *
 * Features:
 * - Visual color stop editing
 * - Relative/absolute modes
 * - Custom labels
 * - Scale import/export
 * - Built-in scale management
 *
 * @extends QDialog
 */
class CCPLUGIN_LIB_API ccColorScaleEditorDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the editor dialog.
	 *
	 * @param[in] manager Color scales manager.
	 * @param[in] mainApp Main application interface.
	 * @param[in] currentScale Scale to edit.
	 * @param[in] parent Parent widget.
	 */
	ccColorScaleEditorDialog(ccColorScalesManager* manager,
	                         ccMainAppInterface*   mainApp,
	                         ccColorScale::Shared  currentScale = ccColorScale::Shared(nullptr),
	                         QWidget*              parent       = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccColorScaleEditorDialog() override;

	/**
	 * @brief Set associated scalar field.
	 *
	 * @param[in] sf Scalar field.
	 */
	void setAssociatedScalarField(ccScalarField* sf);

	/**
	 * @brief Set active scale.
	 *
	 * @param[in] currentScale Scale to edit.
	 */
	void setActiveScale(ccColorScale::Shared currentScale);

	/**
	 * @brief Get the active scale.
	 *
	 * @return Currently edited scale.
	 */
	ccColorScale::Shared getActiveScale()
	{
		return m_colorScale;
	}

  protected slots:
	/**
	 * @brief Handle color scale change.
	 *
	 * @param[in] index New scale index.
	 */
	void colorScaleChanged(int index);

	/**
	 * @brief Handle mode change.
	 *
	 * @param[in] index Mode index.
	 */
	void relativeModeChanged(int index);

	/**
	 * @brief Handle step selection.
	 *
	 * @param[in] index Selected step.
	 */
	void onStepSelected(int index);

	/**
	 * @brief Handle step modification.
	 *
	 * @param[in] index Modified step.
	 */
	void onStepModified(int index);

	/**
	 * @brief Delete selected step.
	 */
	void deletecSelectedStep();

	/**
	 * @brief Change selected step color.
	 */
	void changeSelectedStepColor();

	/**
	 * @brief Change selected step value.
	 *
	 * @param[in] value New value.
	 */
	void changeSelectedStepValue(double value);

	/**
	 * @brief Handle custom labels change.
	 */
	void onCustomLabelsListChanged();

	/**
	 * @brief Toggle custom labels.
	 *
	 * @param[in] state Show state.
	 */
	void toggleCustomLabelsList(bool state);

	/**
	 * @brief Copy current scale.
	 */
	void copyCurrentScale();

	/**
	 * @brief Save current scale.
	 *
	 * @return true on success.
	 */
	bool saveCurrentScale();

	/**
	 * @brief Delete current scale.
	 */
	void deleteCurrentScale();

	/**
	 * @brief Rename current scale.
	 */
	void renameCurrentScale();

	/**
	 * @brief Export current scale.
	 */
	void exportCurrentScale();

	/**
	 * @brief Import scale.
	 */
	void importScale();

	/**
	 * @brief Create new scale.
	 */
	void createNewScale();

	/**
	 * @brief Apply changes.
	 */
	void onApply();

	/**
	 * @brief Close dialog.
	 */
	void onClose();

  protected:
	/**
	 * @brief Update main combo box.
	 */
	void updateMainComboBox();

	/**
	 * @brief Set modification flag.
	 *
	 * @param[in] state Modified state.
	 */
	void setModified(bool state);

	/**
	 * @brief Check if can change scale.
	 *
	 * @return true if user allows change.
	 */
	bool canChangeCurrentScale();

	/**
	 * @brief Check if in relative mode.
	 *
	 * @return true if relative.
	 */
	bool isRelativeMode() const;

	/**
	 * @brief Set scale mode.
	 *
	 * @param[in] isRelative Use relative mode.
	 */
	void setScaleModeToRelative(bool isRelative);

	/**
	 * @brief Check custom labels.
	 *
	 * @param[in] showWarnings Show warnings.
	 *
	 * @return true if valid.
	 */
	bool checkCustomLabelsList(bool showWarnings);

	/**
	 * @brief Export custom labels.
	 *
	 * @param[out] labels Label set.
	 *
	 * @return Error message or empty.
	 */
	QString exportCustomLabelsList(ccColorScale::LabelSet& labels) const;

  private:
	//! Color scales manager
	ccColorScalesManager* m_manager;

	//! Current color scale
	ccColorScale::Shared m_colorScale;

	//! Editor widget
	ccColorScaleEditorWidget* m_scaleWidget;

	//! Associated scalar field
	ccScalarField* m_associatedSF;

	//! Modified flag
	bool m_modified;

	//! Min absolute value
	double m_minAbsoluteVal;

	//! Max absolute value
	double m_maxAbsoluteVal;

	//! Main application interface
	ccMainAppInterface* m_mainApp;

	//! UI definition
	Ui::ColorScaleEditorDlg* m_ui;
};
