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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file ccSectionExtractionSubDlg.h
 *
 * @brief Section extraction sub-dialog
 *
 * Dialog for section generation parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccEnvelopeExtractor.h"

// Qt
#include <QDialog>
#include <ui_sectionExtractionSubDlg.h>

/**
 * @brief Section extraction sub-dialog
 *
 * Configure section generation parameters.
 */
class ccSectionExtractionSubDlg : public QDialog
    , public Ui::SectionExtractionSubDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccSectionExtractionSubDlg(QWidget* parent = nullptr);

	/// Set active section count
	void setActiveSectionCount(int count);

	/// Set section thickness
	void setSectionThickness(double t);
	/// Get section thickness
	double getSectionThickness() const;

	/// Get max edge length
	double getMaxEdgeLength() const;
	/// Set max edge length
	void setMaxEdgeLength(double l);

	/// Get envelope type
	ccEnvelopeExtractor::EnvelopeType getEnvelopeType() const;

	/// Get extract clouds flag
	bool extractClouds() const;
	/// Set extract clouds
	void doExtractClouds(bool state);
	/// Get extract envelopes flag
	bool extractEnvelopes() const;
	/// Set extract envelopes
	void doExtractEnvelopes(bool state, ccEnvelopeExtractor::EnvelopeType type);

	/// Get split envelopes flag
	bool splitEnvelopes() const;
	/// Set split envelopes
	void doSplitEnvelopes(bool state);

	/// Get multipass flag
	bool useMultiPass() const;
	/// Set multipass
	void doUseMultiPass(bool state);

	/// Get visual debug mode
	bool visualDebugMode() const;
};
