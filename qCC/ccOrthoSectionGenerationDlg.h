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

#ifndef CC_ORTHO_SECTION_GENERATION_DIALOG_HEADER
#define CC_ORTHO_SECTION_GENERATION_DIALOG_HEADER

/**
 * @file ccOrthoSectionGenerationDlg.h
 *
 * @brief Ortho section generation dialog for creating sections along a path.
 *
 * @details Dialog for configuring orthogonal section generation
 * parameters. Orthogonal sections are cross-sectional slices
 * extracted perpendicular to a path (e.g., along a polyline).
 *
 * Used for:
 * - Generating profile sections along a survey line
 * - Creating cross-sections for structural analysis
 * - Extracting slices perpendicular to a path
 *
 * Parameters:
 * - Path length for normalization
 * - Generation step (spacing between sections)
 * - Section width (width of each section)
 * - Auto-save option
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccClippingBoxTool
 */

#include <QDialog>
#include <ui_orthoSectionGenerationDlg.h>

/**
 * @brief Dialog for generating orthogonal sections.
 *
 * @details Provides a UI for setting up orthogonal section
 * generation along a path.
 *
 * Features:
 * - Configurable section spacing (step)
 * - Section width setting
 * - Path length for normalization
 * - Auto-save option for generated sections
 *
 * @extends QDialog
 * @extends Ui::OrthoSectionGenerationDlg
 */
class ccOrthoSectionGenerationDlg : public QDialog
    , public Ui::OrthoSectionGenerationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the ortho section generation dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccOrthoSectionGenerationDlg(QWidget* parent = nullptr);

	/**
	 * @brief Set the path length.
	 *
	 * @param[in] l Path length for normalization.
	 */
	void setPathLength(double l);

	/**
	 * @brief Set auto-save and remove mode.
	 *
	 * @param[in] state Auto-save state.
	 */
	void setAutoSaveAndRemove(bool state);

	/**
	 * @brief Check if auto-save is enabled.
	 *
	 * @return true if auto-save is on.
	 */
	bool autoSaveAndRemove() const;

	/**
	 * @brief Set the generation step.
	 *
	 * @param[in] s Spacing between sections.
	 */
	void setGenerationStep(double s);

	/**
	 * @brief Set the section width.
	 *
	 * @param[in] w Width of each section.
	 */
	void setSectionsWidth(double w);

	/**
	 * @brief Get the generation step.
	 *
	 * @return Spacing between sections.
	 */
	double getGenerationStep() const;

	/**
	 * @brief Get the section width.
	 *
	 * @return Width of each section.
	 */
	double getSectionsWidth() const;

  protected slots:
	/**
	 * @brief Handle step value change.
	 *
	 * @param[in] value New step value.
	 */
	void onStepChanged(double value);

  protected:
	//! Path length
	double m_pathLength;
};

#endif // CC_ORTHO_SECTION_GENERATION_DIALOG_HEADER
