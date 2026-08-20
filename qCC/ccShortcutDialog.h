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
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        //
// ##########################################################################

#ifndef CC_SHORTCUTDIALOG_H
#define CC_SHORTCUTDIALOG_H

/**
 * @file ccShortcutDialog.h
 *
 * @brief Keyboard shortcut configuration dialogs.
 *
 * @details Dialogs for viewing and editing keyboard shortcuts
 * for application actions.
 *
 * Features:
 * - View all registered shortcuts
 * - Edit shortcuts by double-clicking
 * - Conflict detection
 * - Restore default shortcuts
 *
 * @author CloudCompare project
 */

#include "ui_shorcutSettings.h"
#include "ui_shortcutEditDialog.h"

/**
 * @brief Dialog for capturing a key sequence.
 *
 * @details Modal dialog that captures keyboard input
 * for assigning to an action.
 *
 * Usage:
 * 1. Dialog opens with focus
 * 2. User presses key combination
 * 3. Key sequence is captured
 * 4. Dialog closes with accepted/rejected
 *
 * @extends QDialog
 */
class ccShortcutEditDialog final : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the shortcut edit dialog.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccShortcutEditDialog(QWidget* parent = nullptr);

	/**
	 * @brief Get the captured key sequence.
	 *
	 * @return Key sequence.
	 */
	QKeySequence keySequence() const;

	/**
	 * @brief Set the current key sequence.
	 *
	 * @param[in] sequence Key sequence.
	 */
	void setKeySequence(const QKeySequence& sequence) const;

	/**
	 * @brief Execute the dialog.
	 *
	 * @return Accepted or rejected.
	 */
	int exec() override;

  private:
	//! UI definition
	Ui_ShortcutEditDialog* m_ui;
};

/**
 * @brief Dialog for viewing and editing all shortcuts.
 *
 * @details Provides a table view of all registered actions
 * and their associated keyboard shortcuts.
 *
 * Features:
 * - Table of action names and shortcuts
 * - Double-click to edit
 * - Conflict detection
 * - Reset to defaults
 *
 * @extends QDialog
 */
class ccShortcutDialog final : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the shortcut dialog.
	 *
	 * @param[in] actions List of actions with shortcuts.
	 * @param[in] parent Parent widget.
	 */
	explicit ccShortcutDialog(const QList<QAction*>& actions, QWidget* parent = nullptr);

	/**
	 * @brief Restore shortcuts from settings.
	 */
	void restoreShortcutsFromQSettings() const;

  private:
	/**
	 * @brief Check for shortcut conflict.
	 *
	 * @param[in] sequence Key sequence to check.
	 *
	 * @return Conflicting action, or nullptr.
	 */
	const QAction* checkConflict(const QKeySequence& sequence) const;

	/**
	 * @brief Handle table item double-click.
	 *
	 * @param[in] item Clicked item.
	 */
	void handleDoubleClick(QTableWidgetItem* item);

  private:
	//! UI definition
	Ui_ShortcutDialog* m_ui;

	//! Edit dialog
	ccShortcutEditDialog* m_editDialog;
};

#endif // CC_SHORTCUTDIALOG_H
