#pragma once

// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        //
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccSectionExtractionSubDlg.h
 *
 * @brief Sub-dialog for section extraction parameters.
 *
 * @details Dialog for configuring section extraction parameters
 * when generating cross-sections from point clouds.
 *
 * Parameters:
 * - **Section thickness**: Width of the section slice
 * - **Max edge length**: Maximum edge for mesh generation
 * - **Envelope type**: Alpha shape, convex hull, or concave hull
 * - **Multi-pass**: Iterative refinement
 * - **Split envelopes**: Split at gaps
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccSectionExtractionTool
 * @see ccEnvelopeExtractor
 */

#include "ccEnvelopeExtractor.h"

// Qt
#include <QDialog>
#include <ui_sectionExtractionSubDlg.h>

/**
 * @brief Sub-dialog for section extraction configuration.
 *
 * @details Provides additional options for section extraction
 * beyond the main ccSectionExtractionTool.
 *
 * @extends QDialog
 */
class ccSectionExtractionSubDlg : public QDialog
    , public Ui::SectionExtractionSubDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the sub-dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccSectionExtractionSubDlg(QWidget* parent = nullptr);

	/**
	 * @brief Set the number of active sections.
	 *
	 * @param[in] count Section count.
	 */
	void setActiveSectionCount(int count);

	/**
	 * @brief Set section thickness.
	 *
	 * @param[in] t Thickness value.
	 */
	void setSectionThickness(double t);

	/**
	 * @brief Get section thickness.
	 *
	 * @return Thickness value.
	 */
	double getSectionThickness() const;

	/**
	 * @brief Get maximum edge length.
	 *
	 * @return Max edge length.
	 */
	double getMaxEdgeLength() const;

	/**
	 * @brief Set maximum edge length.
	 *
	 * @param[in] l Max edge length.
	 */
	void setMaxEdgeLength(double l);

	/**
	 * @brief Get envelope type.
	 *
	 * @return Envelope type.
	 */
	ccEnvelopeExtractor::EnvelopeType getEnvelopeType() const;

	/**
	 * @brief Check if extracting clouds.
	 *
	 * @return true if extracting.
	 */
	bool extractClouds() const;

	/**
	 * @brief Set cloud extraction state.
	 *
	 * @param[in] state Enable/disable.
	 */
	void doExtractClouds(bool state);

	/**
	 * @brief Check if extracting envelopes.
	 *
	 * @return true if extracting.
	 */
	bool extractEnvelopes() const;

	/**
	 * @brief Set envelope extraction state.
	 *
	 * @param[in] state Enable/disable.
	 * @param[in] type Envelope type.
	 */
	void doExtractEnvelopes(bool state, ccEnvelopeExtractor::EnvelopeType type);

	/**
	 * @brief Check if splitting envelopes.
	 *
	 * @return true if splitting.
	 */
	bool splitEnvelopes() const;

	/**
	 * @brief Set split envelopes state.
	 *
	 * @param[in] state Enable/disable.
	 */
	void doSplitEnvelopes(bool state);

	/**
	 * @brief Check if using multi-pass.
	 *
	 * @return true if multi-pass.
	 */
	bool useMultiPass() const;

	/**
	 * @brief Set multi-pass state.
	 *
	 * @param[in] state Enable/disable.
	 */
	void doUseMultiPass(bool state);

	/**
	 * @brief Check if visual debug mode.
	 *
	 * @return true if debug mode.
	 */
	bool visualDebugMode() const;
};
