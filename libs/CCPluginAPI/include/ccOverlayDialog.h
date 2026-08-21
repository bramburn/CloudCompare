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
 * @file ccOverlayDialog.h
 *
 * @brief Overlay dialog interface
 *
 * Base class for floating dialogs that appear above 3D views during
 * interactive operations. Overlay dialogs are used by plugins for tasks
 * like manual registration, segmentation, and measurement.
 *
 * Key behaviors:
 * - Frameless, tool-window style (floating above GL windows)
 * - Auto-repositioned when the main window is resized
 * - Linked to a specific ccGLWindowInterface
 * - Escape key closes the dialog
 * - Can intercept specific keyboard shortcuts
 *
 * Lifecycle: linkWith() → start() → [user interaction] → stop()
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see ccMainAppInterface::registerOverlayDialog()
 */
#include "CCPluginAPI.h"

// Qt
#include <QDialog>
#include <QList>

class ccGLWindowInterface;

/**
 * @brief Overlay dialog interface
 *
 * Floating dialog that appears above 3D views. Subclasses implement
 * interactive operations (e.g., manual point picking, segmentation).
 *
 * The dialog is shown via start() and hidden via stop(). The linked
 * GL window is notified via registerOverlayDialog() / unregisterOverlayDialog()
 * so the application can manage its lifecycle alongside the window.
 *
 * @note The dialog is frameless by default (Qt::FramelessWindowHint | Qt::Tool)
 *       to float above GL windows without taking focus from them.
 */
class CCPLUGIN_LIB_API ccOverlayDialog : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct an overlay dialog
	 *
	 * Creates a frameless tool window. An Escape shortcut is automatically
	 * registered to close the dialog.
	 *
	 * @param[in] parent Parent widget
	 * @param[in] flags  Window flags (default: frameless + tool)
	 */
	explicit ccOverlayDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Tool);

	/**
	 * @brief Destructor
	 *
	 * Calls onLinkedWindowDeletion() to cleanly detach from the GL window.
	 */
	~ccOverlayDialog() override;

	/**
	 * @brief Link this dialog to a GL window
	 *
	 * Establishes a relationship between this dialog and a GL window:
	 * - The dialog's position tracks the window's position on resize
	 * - The dialog receives events from the window (keyboard, close)
	 * - When the window closes, the dialog is automatically stopped
	 *
	 * May only be called when the dialog is not running (not started or
	 * already stopped).
	 *
	 * @param[in] win GL window to link with (nullptr to detach)
	 * @return true on success; false if dialog is currently running
	 */
	virtual bool linkWith(ccGLWindowInterface* win);

	/**
	 * @brief Start the interactive process
	 *
	 * Called by the plugin to begin the interaction. Shows the dialog
	 * and sets m_processing = true. Subclasses should override to
	 * set up their state before calling this base method.
	 *
	 * @return true if started successfully; false if already running
	 * @see stop()
	 */
	virtual bool start();

	/**
	 * @brief Stop the interactive process
	 *
	 * Hides the dialog, detaches from the GL window, and emits
	 * processFinished(). Called automatically when the linked window
	 * is closed.
	 *
	 * @param[in] accepted true if the user accepted the result; false if cancelled
	 * @see start()
	 */
	virtual void stop(bool accepted);

	// Reimplemented from QDialog
	/**
	 * @brief Handle rejection (Escape key or dialog close)
	 *
	 * Calls QDialog::reject() then stop(false).
	 */
	void reject() override;

	/**
	 * @brief Register a keyboard shortcut to be intercepted
	 *
	 * Keys registered here are consumed by the dialog and trigger
	 * the shortcutTriggered() signal instead of being passed to
	 * the GL window.
	 *
	 * @param[in] key Qt key code to intercept
	 */
	void addOverriddenShortcut(Qt::Key key);

	/**
	 * @brief Check if the process is currently running
	 * @return true if start() has been called and stop() has not yet been called
	 * @see start(), stop()
	 */
	bool started() const
	{
		return m_processing;
	}

  signals:

	/**
	 * @brief Emitted when the interactive process finishes
	 * @param[in] accepted true if user accepted the result, false if cancelled
	 */
	void processFinished(bool accepted);

	/**
	 * @brief Emitted when an intercepted shortcut is triggered
	 * @param[in] key Qt key code that was intercepted
	 */
	void shortcutTriggered(int key);

	/**
	 * @brief Emitted when the dialog is shown
	 */
	void shown();

  protected:
	/**
	 * @brief Called when the linked GL window is about to be deleted
	 *
	 * Stops the process if running and detaches from the window.
	 * Subclasses may override to perform additional cleanup.
	 *
	 * @param[in] object Pointer to the GL window being deleted
	 */
	virtual void onLinkedWindowDeletion(ccGLWindowInterface* object = nullptr);

  protected:
	// From QObject
	/**
	 * @brief Event filter for keyboard and show events
	 *
	 * Handles:
	 * - KeyPress: routes overridden shortcuts to shortcutTriggered();
	 *             Escape key closes the dialog
	 * - Show: emits the shown() signal
	 * - All other events pass through to QDialog::eventFilter()
	 *
	 * @param[in] obj Object receiving the event
	 * @param[in] e   Event
	 * @return true if the event was handled
	 */
	bool eventFilter(QObject* obj, QEvent* e) override;

	//! Linked GL window (may be nullptr)
	ccGLWindowInterface* m_associatedWin;

	//! Whether the interactive process is currently running
	bool m_processing;

	//! List of keyboard shortcuts intercepted by this dialog
	QList<int> m_overriddenKeys;
};
