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
 * @brief Ortho section generation dialog
 *
 * Dialog for generating orthogonal sections.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QDialog>
#include <ui_orthoSectionGenerationDlg.h>

/**
 * @brief Ortho section generation dialog
 *
 * Generate orthogonal sections along a path.
 */
class ccOrthoSectionGenerationDlg : public QDialog
    , public Ui::OrthoSectionGenerationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccOrthoSectionGenerationDlg(QWidget* parent = nullptr);

	/// Set path length
	void setPathLength(double l);

	/// Set auto save and remove
	void setAutoSaveAndRemove(bool state);
	/// Get auto save and remove
	bool autoSaveAndRemove() const;

	/// Set generation step
	void setGenerationStep(double s);
	/// Set sections width
	void setSectionsWidth(double w);

	/// Get generation step
	double getGenerationStep() const;
	/// Get sections width
	double getSectionsWidth() const;

  protected:
	/// Handle step changed
	void onStepChanged(double);

  protected:
	/// Path length
	double m_pathLength;
};

#endif // CC_ORTHO_SECTION_GENERATION_DIALOG_HEADER
