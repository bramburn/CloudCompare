#ifndef CC_SHORTCUTDIALOG_H
#define CC_SHORTCUTDIALOG_H

/**
 * @file ccShortcutDialog.h
 *
 * @brief Shortcut dialog
 *
 * Dialogs for editing keyboard shortcuts.
 */

#include "ui_shorcutSettings.h"
#include "ui_shortcutEditDialog.h"

/**
 * @brief Shortcut edit dialog
 *
 * Capture key sequences for editing shortcuts.
 */
class ccShortcutEditDialog final : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] parent Parent widget
	 */
	explicit ccShortcutEditDialog(QWidget* parent = nullptr);

	/// Get key sequence
	QKeySequence keySequence() const;

	/// Set key sequence
	void setKeySequence(const QKeySequence& sequence) const;

	/// Execute dialog
	int exec() override;

  private:
	Ui_ShortcutEditDialog* m_ui;
};

/**
 * @brief Shortcut dialog
 *
 * List and edit shortcuts for known actions.
 */
class ccShortcutDialog final : public QDialog
{
	Q_OBJECT
  public:
	/**
	 * @brief Create dialog
	 * @param[in] actions Available actions
	 * @param[in] parent Parent widget
	 */
	explicit ccShortcutDialog(const QList<QAction*>& actions, QWidget* parent = nullptr);

	/// Restore shortcuts from settings
	void restoreShortcutsFromQSettings() const;

  private:
	/// Check for conflict
	const QAction* checkConflict(const QKeySequence& sequence) const;
	/// Handle double click
	void handleDoubleClick(QTableWidgetItem* item);

	Ui_ShortcutDialog*    m_ui;
	ccShortcutEditDialog* m_editDialog;
};

#endif // CC_SHORTCUTDIALOG_H
