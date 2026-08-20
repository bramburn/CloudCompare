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
 * @file ccEnvelopeExtractorDlg.h
 *
 * @brief Envelope extractor debug dialog for visualizing the extraction algorithm.
 *
 * @details Debug visualization dialog for the envelope extraction algorithm.
 *
 * This dialog provides step-by-step visualization of the concave hull
 * extraction process, allowing users to:
 * - See intermediate algorithm steps
 * - Pause and resume extraction
 * - Skip ahead
 * - View the progress in real-time
 *
 * This is primarily used for debugging and understanding the algorithm,
 * not for normal use.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccEnvelopeExtractor
 */

// Qt
#include <QDialog>
#include <QEventLoop>

// qCC_db
#include <ccBBox.h>

// GUI
#include <ui_envelopeExtractorDlg.h>

class ccGLWindowInterface;
class ccHObject;

/**
 * @brief Debug dialog for envelope extraction visualization.
 *
 * @details Provides step-by-step visualization of the envelope
 * extraction algorithm for debugging and understanding.
 *
 * Features:
 * - Message display with step descriptions
 * - Wait for user input between steps
 * - Skip button to bypass remaining steps
 * - Real-time 2D/3D visualization
 * - Zoom to regions of interest
 *
 * @extends QDialog
 * @extends Ui::EnvelopeExtractorDlg
 */
class ccEnvelopeExtractorDlg : public QDialog
    , public Ui::EnvelopeExtractorDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the debug dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccEnvelopeExtractorDlg(QWidget* parent = nullptr);

	/**
	 * @brief Initialize the display.
	 */
	void init();

	/**
	 * @brief Display a message.
	 *
	 * @param[in] message Message to display.
	 * @param[in] waitForUserConfirmation Pause and wait.
	 */
	void displayMessage(QString message, bool waitForUserConfirmation = false);

	/**
	 * @brief Wait for user action.
	 *
	 * @param[in] defaultDelay_ms Default delay before continuing.
	 */
	void waitForUser(unsigned defaultDelay_ms = 100);

	/**
	 * @brief Get the associated GL window.
	 * @return Pointer to GL window.
	 */
	inline ccGLWindowInterface* win()
	{
		return m_glWindow;
	}

	/**
	 * @brief Zoom to a 2D region.
	 *
	 * @param[in] bbox Bounding box to zoom to.
	 */
	void zoomOn(const ccBBox& bbox);

	/**
	 * @brief Refresh the display.
	 */
	void refresh();

	/**
	 * @brief Add entity to display.
	 *
	 * @param[in] obj Entity to add.
	 * @param[in] noDependency No dependency management.
	 */
	void addToDisplay(ccHObject* obj, bool noDependency = true);

	/**
	 * @brief Remove entity from display.
	 *
	 * @param[in] obj Entity to remove.
	 */
	void removFromDisplay(ccHObject* obj);

	/**
	 * @brief Check if skipped.
	 * @return true if user clicked skip.
	 */
	bool isSkipped() const;

  protected slots:
	/**
	 * @brief Handle skip button click.
	 */
	void onSkipButtonClicked();

  protected:
	//! Skip flag
	bool m_skipped;

	//! Event loop for waiting
	QEventLoop m_loop;

	//! Associated GL window
	ccGLWindowInterface* m_glWindow;
};
