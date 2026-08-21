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
 * @file ccOverlayDialog.cpp
 *
 * @brief Overlay dialog implementation
 *
 * Implements the overlay dialog lifecycle: linking to a GL window,
 * keyboard shortcut interception, and the start/stop state machine.
 *
 * @see ccOverlayDialog
 */

#include "ccOverlayDialog.h"

// qCC_glWindow
#include <ccGLWindowInterface.h>

// qCC_db
#include <ccLog.h>

// Qt
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QShortcut>

// system
#include <cassert>

// ccOverlayDialog::ccOverlayDialog
/**
 * @brief Construct an overlay dialog
 *
 * Registers an Escape-key shortcut that closes the dialog. The shortcut
 * is owned by this object so it is automatically cleaned up on destruction.
 *
 * @param[in] parent Parent widget
 * @param[in] flags Window flags (frameless + tool for floating above GL windows)
 */
ccOverlayDialog::ccOverlayDialog(QWidget* parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , m_associatedWin(nullptr)
    , m_processing(false)
{
	// Escape key closes the dialog
	QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	connect(escShortcut, &QShortcut::activated, [this]() { close(); });
}

// ccOverlayDialog::~ccOverlayDialog
/**
 * @brief Destructor
 *
 * Detaches from the GL window before destruction to avoid dangling pointers.
 */
ccOverlayDialog::~ccOverlayDialog()
{
	onLinkedWindowDeletion();
}

// ccOverlayDialog::linkWith
/**
 * @brief Link (or unlink) this dialog to a GL window
 *
 * When linking to a new window:
 * 1. Uninstalls the event filter from all top-level widgets (if previously linked)
 * 2. Disconnects from the previous window's signal emitter
 * 3. Installs the event filter on all current top-level widgets
 * 4. Connects to the new window's aboutToClose signal
 *
 * When unlinking (win == nullptr):
 * - Removes the event filter from all top-level widgets
 * - Disconnects from the current window
 *
 * @param[in] win GL window to link with (nullptr to detach)
 * @return true on success; false if dialog is currently running
 */
bool ccOverlayDialog::linkWith(ccGLWindowInterface* win)
{
	if (m_processing)
	{
		ccLog::Warning("[ccOverlayDialog] Can't change associated window while running/displayed!");
		return false;
	}

	// No change needed
	if (m_associatedWin == win)
	{
		return true;
	}

	// Detach from the old window (if any)
	if (m_associatedWin)
	{
		{
			QWidgetList topWidgets = QApplication::topLevelWidgets();
			foreach (QWidget* widget, topWidgets)
			{
				widget->removeEventFilter(this);
			}
		}
		m_associatedWin->signalEmitter()->disconnect(this);
		m_associatedWin = nullptr;
	}

	// Attach to the new window
	m_associatedWin = win;
	if (m_associatedWin)
	{
		QWidgetList topWidgets = QApplication::topLevelWidgets();
		foreach (QWidget* widget, topWidgets)
		{
			widget->installEventFilter(this);
		}
		connect(m_associatedWin->signalEmitter(),
		        &ccGLWindowSignalEmitter::aboutToClose,
		        this,
		        &ccOverlayDialog::onLinkedWindowDeletion);
	}

	return true;
}

// ccOverlayDialog::onLinkedWindowDeletion
/**
 * @brief Handle the linked window being destroyed
 *
 * Called when the connected GL window emits aboutToClose. If the
 * dialog is currently running, calls stop(false) first, then
 * detaches from the window.
 *
 * @param[in] object Pointer to the window being deleted
 */
void ccOverlayDialog::onLinkedWindowDeletion(ccGLWindowInterface* object)
{
	if (m_associatedWin == object)
	{
		if (m_processing)
		{
			stop(false);
		}
		linkWith(nullptr);
	}
	else
	{
		assert(false);
	}
}

// ccOverlayDialog::start
/**
 * @brief Start the interactive process
 *
 * Sets m_processing = true and shows the dialog. Subclasses
 * should perform their own initialization before calling this.
 *
 * @return true if started; false if already running
 */
bool ccOverlayDialog::start()
{
	if (m_processing)
		return false;

	m_processing = true;
	show();

	return true;
}

// ccOverlayDialog::stop
/**
 * @brief Stop the interactive process
 *
 * Hides the dialog, detaches from the GL window, and emits
 * processFinished() with the accepted flag.
 *
 * @param[in] accepted true if the user accepted the result
 */
void ccOverlayDialog::stop(bool accepted)
{
	m_processing = false;
	hide();
	linkWith(nullptr);
	processFinished(accepted);
}

// ccOverlayDialog::reject
/**
 * @brief Handle dialog rejection
 *
 * Called when the user presses Escape or closes the dialog.
 * Delegates to QDialog::reject() then calls stop(false).
 */
void ccOverlayDialog::reject()
{
	QDialog::reject();
	stop(false);
}

// ccOverlayDialog::addOverriddenShortcut
/**
 * @brief Register a keyboard shortcut for interception
 *
 * Keys registered here are consumed by the event filter and
 * trigger the shortcutTriggered() signal rather than being
 * passed to the GL window.
 *
 * @param[in] key Qt key code to intercept
 */
void ccOverlayDialog::addOverriddenShortcut(Qt::Key key)
{
	m_overriddenKeys.push_back(key);
}

// ccOverlayDialog::eventFilter
/**
 * @brief Filter events from top-level widgets
 *
 * Intercepts:
 * - KeyPress events matching an overridden shortcut → emit shortcutTriggered()
 * - Escape key → close the dialog
 * - Show events → emit shown()
 *
 * All other events pass to QDialog::eventFilter().
 *
 * @param[in] obj Object that received the event
 * @param[in] e   Event
 * @return true if the event was consumed
 */
bool ccOverlayDialog::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);

		// Check overridden shortcuts first
		if (m_overriddenKeys.contains(keyEvent->key()))
		{
			shortcutTriggered(keyEvent->key());
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Escape)
		{
			close();
			return true;
		}
		else
		{
			return QDialog::eventFilter(obj, e);
		}
	}
	else
	{
		if (e->type() == QEvent::Show)
		{
			shown();
		}
		return QDialog::eventFilter(obj, e);
	}
}
