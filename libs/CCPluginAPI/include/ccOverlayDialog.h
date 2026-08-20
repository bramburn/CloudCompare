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
 * @file ccOverlayDialog.h
 *
 * @brief Overlay dialog base class for interactive tools.
 *
 * @details Base class for floating dialogs that appear above 3D views
 * during interactive operations like segmentation, transformation,
 * section extraction, etc.
 *
 * ## Overview
 *
 * Overlay dialogs are frameless, tool-style windows that:
 * - Float above 3D OpenGL views
 * - Are repositioned automatically on window resize
 * - Can override keyboard shortcuts
 * - Track the linked GL window
 *
 * ## Lifecycle
 *
 * 1. Create dialog
 * 2. Call linkWith() to attach to a GL window
 * 3. Call start() to begin the interactive process
 * 4. User interacts with 3D view and dialog
 * 5. Call stop() to end (accepted or rejected)
 *
 * ## Example
 *
 * @code
 * class MyTool : public ccOverlayDialog
 * {
 * public:
 *     MyTool(QWidget* parent) : ccOverlayDialog(parent) {}
 *
 *     bool linkWith(ccGLWindowInterface* win) override
 *     {
 *         // Connect to window signals
 *         return ccOverlayDialog::linkWith(win);
 *     }
 *
 *     bool start() override
 *     {
 *         m_processing = true;
 *         return true;
 *     }
 *
 *     void stop(bool accepted) override
 *     {
 *         m_processing = false;
 *         if (accepted) {
 *             // Apply changes
 *         }
 *         ccOverlayDialog::stop(accepted);
 *     }
 * };
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccMainAppInterface for dialog registration
 */

#pragma once

#include "CCPluginAPI.h"

// Qt
#include <QDialog>
#include <QList>

class ccGLWindowInterface;

/**
 * @brief Base class for overlay dialogs.
 *
 * @details Provides a frameless dialog that floats above 3D views
 * for interactive operations.
 *
 * Features:
 * - Frameless, tool-style window
 * - Automatic repositioning on resize
 * - Keyboard shortcut override
 * - GL window state tracking
 *
 * @extends QDialog
 */
class CCPLUGIN_LIB_API ccOverlayDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct an overlay dialog.
	 *
	 * @param[in] parent Parent widget.
	 * @param[in] flags Window flags (default: frameless tool).
	 */
	explicit ccOverlayDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Tool);

	/**
	 * @brief Destructor.
	 */
	~ccOverlayDialog() override;

	/**
	 * @brief Link with a 3D window.
	 *
	 * @param[in] win Window to link with.
	 *
	 * @return true on success.
	 */
	virtual bool linkWith(ccGLWindowInterface* win);

	/**
	 * @brief Start the interactive process.
	 *
	 * @return true on success.
	 */
	virtual bool start();

	/**
	 * @brief Stop the interactive process.
	 *
	 * @param[in] accepted Whether to apply changes.
	 */
	virtual void stop(bool accepted);

	/**
	 * @brief Handle dialog rejection.
	 */
	void reject() override;

	/**
	 * @brief Add a keyboard shortcut override.
	 *
	 * @param[in] key Key to override.
	 */
	void addOverriddenShortcut(Qt::Key key);

	/**
	 * @brief Check if process is active.
	 *
	 * @return true if started.
	 */
	bool started() const
	{
		return m_processing;
	}

  signals:
	/**
	 * @brief Emitted when process finishes.
	 *
	 * @param[in] accepted Result state.
	 */
	void processFinished(bool accepted);

	/**
	 * @brief Emitted when overridden shortcut triggers.
	 *
	 * @param[in] key Key code.
	 */
	void shortcutTriggered(int key);

	/**
	 * @brief Emitted when dialog is shown.
	 */
	void shown();

  protected slots:
	/**
	 * @brief Handle linked window deletion.
	 *
	 * @param[in] object Deleted window.
	 */
	virtual void onLinkedWindowDeletion(ccGLWindowInterface* object = nullptr);

  protected:
	/**
	 * @brief Event filter for keyboard handling.
	 *
	 * @param[in] obj Object.
	 * @param[in] e Event.
	 *
	 * @return true if handled.
	 */
	bool eventFilter(QObject* obj, QEvent* e) override;

  private:
	//! Linked GL window
	ccGLWindowInterface* m_associatedWin;

	//! Processing state
	bool m_processing;

	//! Overridden keys
	QList<int> m_overriddenKeys;
};
