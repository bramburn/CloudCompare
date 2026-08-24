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
 * @file ccConsole.h
 *
 * @brief Console/logging widget
 *
 * Console widget and logging infrastructure for CloudCompare.
 * Provides a central message hub that routes log messages to:
 * - A QListWidget display widget
 * - An optional log file
 * - System stdout/stderr (debug mode)
 * - Qt message handlers (qDebug, qWarning, etc.)
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// qCC_db
#include <ccLog.h>

// Qt
#include <QFile>
#include <QListWidget>
#include <QMutex>
#include <QTimer>

class MainWindow;
class QTextStream;

/**
 * @brief Custom list widget with copy support
 *
 * Extends QListWidget to intercept copy operations (Ctrl+C / Copy shortcut).
 * When the user copies, all selected items are joined with newlines and
 * placed on the system clipboard.
 *
 * @see ccConsole
 */
class ccCustomQListWidget : public QListWidget
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct a custom list widget
	 * @param parent Parent widget (optional)
	 */
	ccCustomQListWidget(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief Handle key press events
	 *
	 * Intercepts the Copy keyboard shortcut. If the pressed key sequence
	 * matches the Copy action, all currently selected items are copied
	 * to the system clipboard as newline-separated plain text.
	 * All other key events are passed to the base class.
	 *
	 * @param event Key event
	 */
	void keyPressEvent(QKeyEvent* event) override;
};

/**
 * @brief Console
 *
 * Central logging console for CloudCompare. Acts as both a singleton
 * instance and a Qt message handler for qDebug/qWarning/etc. messages.
 *
 * The console manages a message queue and periodically flushes messages
 * to:
 * - A QListWidget display (textDisplay)
 * - A log file (optional)
 * - System stdout/stderr (debug mode only)
 *
 * In release mode, no messages are output unless a textDisplay widget
 * is set. In debug mode, all messages are also sent to the system
 * console via std::cout/std::cerr.
 *
 * Error messages are shown in a blocking QMessageBox if a parentWidget
 * is set.
 *
 * @note Use ccLog::Print, ccLog::Warning, ccLog::Error, etc. to log.
 * @see ccLog
 */
class ccConsole : public QObject
    , public ccLog
{
	Q_OBJECT

  public:
	/**
	 * @brief Destructor
	 *
	 * Closes any open log file and releases resources.
	 */
	~ccConsole() override;

	/**
	 * @brief Initialize the console
	 *
	 * Sets up the console singleton and optionally associates it with
	 * a QListWidget for on-screen output. Must be called exactly once
	 * before any logging occurs.
	 *
	 * In release mode, no message will be output if no textDisplay
	 * widget is defined. Error messages will only appear in a blocking
	 * QMessageBox if a parentWidget is defined.
	 *
	 * In debug mode, all messages are sent to the system console (printf)
	 * regardless of the widget settings.
	 *
	 * @param textDisplay       QListWidget for on-screen message display (optional)
	 * @param parentWidget      Parent widget for error dialogs (optional)
	 * @param parentWindow      Main window for console visibility management (optional)
	 * @param redirectToStdOut Redirect log messages to system stdout/stderr (optional)
	 */
	static void Init(QListWidget* textDisplay = nullptr,
	                 QWidget* parentWidget = nullptr,
	                 MainWindow* parentWindow = nullptr,
	                 bool redirectToStdOut = false);

	/**
	 * @brief Get the unique static console instance
	 *
	 * Returns the singleton ccConsole instance, creating it on-demand
	 * if autoInit is true and no instance exists yet.
	 *
	 * @param autoInit If true, automatically create the instance with no
	 *                associated widget if it doesn't exist. Default is true.
	 * @return Pointer to the ccConsole singleton, or nullptr if autoInit
	 *         is false and no instance has been created.
	 */
	static ccConsole* TheInstance(bool autoInit = true);

	/**
	 * @brief Release the console singleton
	 *
	 * Destroys the console instance and unregisters it from ccLog.
	 * Optionally flushes any pending messages before shutdown.
	 *
	 * @param flush If true, flush any pending messages before releasing.
	 *              Default is true.
	 */
	static void ReleaseInstance(bool flush = true);

	/**
	 * @brief Set the console refresh cycle
	 *
	 * Controls how often the message queue is flushed to the display
	 * and log file. Must be strictly positive. The change takes effect
	 * immediately if auto-refresh is currently active.
	 *
	 * @param cycle_ms Refresh interval in milliseconds. Default is 1000ms.
	 *                 If invalid (<=0), a warning is logged and the
	 *                 change is ignored.
	 */
	static void SetRefreshCycle(int cycle_ms = 1000);

	/**
	 * @brief Enable or disable automatic message flushing
	 *
	 * When enabled, the console starts an internal timer that
	 * periodically calls refresh() to flush the message queue.
	 * The interval is controlled by SetRefreshCycle().
	 *
	 * @param state True to enable auto-refresh, false to disable
	 * @see refresh(), SetRefreshCycle(), autoRefresh()
	 */
	void setAutoRefresh(bool state);

	/**
	 * @brief Check if auto-refresh is active
	 * @return true if the internal timer is running, false otherwise
	 * @see setAutoRefresh()
	 */
	bool autoRefresh() const;

	/**
	 * @brief Set the console log file
	 *
	 * Opens a file for writing console output. All messages (regardless
	 * of level) are written to the file in chronological order with
	 * timestamps. Calling with an empty filename closes any open log
	 * file without opening a new one.
	 *
	 * @param filename Path to the log file. Empty string closes the
	 *                 current log file without opening a new one.
	 * @return true if the file was opened successfully (or filename
	 *         is empty), false on error.
	 */
	bool setLogFile(const QString& filename);

	/**
	 * @brief Enable or disable Qt message forwarding
	 *
	 * When enabled, Qt's qDebug, qWarning, qCritical, and qFatal
	 * messages are forwarded to the ccConsole logging system.
	 * This setting is persisted to the application settings.
	 *
	 * @param state true to enable Qt message forwarding, false to ignore
	 * @see QtMessagesEnabled(), EnableQtMessages()
	 */
	static void EnableQtMessages(bool state);

	/**
	 * @brief Check if Qt message forwarding is enabled
	 * @return true if Qt messages are being forwarded to the console
	 * @see EnableQtMessages()
	 */
	static bool QtMessagesEnabled()
	{
		return s_showQtMessagesInConsole;
	}

	/**
	 * @brief Get the parent widget
	 * @return Pointer to the parent widget, or nullptr if none was set
	 */
	inline QWidget* parentWidget()
	{
		return m_parentWidget;
	}

  public:
	/**
	 * @brief Flush the message queue
	 *
	 * Processes all messages currently in the queue:
	 * - Writes them to the log file (if open)
	 * - Adds them as items to the QListWidget (if set)
	 * - Clears the queue
	 *
	 * Message color in the widget reflects severity:
	 * - Red: Error messages
	 * - Magenta: Warning messages
	 * - Blue (debug only): Debug messages
	 *
	 * @see setAutoRefresh(), logMessage()
	 */
	void refresh();

  protected:
	/**
	 * @brief Protected constructor
	 *
	 * Constructor is protected to enforce singleton usage through
	 * TheInstance() and Init(). Direct construction is prohibited.
	 */
	ccConsole();

	/**
	 * @brief Handle an incoming log message
	 *
	 * Called by ccLog when a message should be logged.
	 * Adds the message to the internal queue for later display.
	 * In debug mode without a textDisplay, prints directly to stdout/stderr.
	 * Shows error messages in a QMessageBox if a parent widget is set.
	 *
	 * @param message Message text
	 * @param level   Log level (see ccLog LOG_* flags)
	 */
	void logMessage(const QString& message, int level) override;

	//! Associated text display widget (may be nullptr)
	QListWidget* m_textDisplay;

	//! Parent widget for error dialogs
	QWidget* m_parentWidget;

	//! Parent window (for console visibility management)
	MainWindow* m_parentWindow;

	//! Mutex for thread-safe access to the message queue
	QMutex m_mutex;

	//! Queue element: message text paired with its log level
	using ConsoleItemType = QPair<QString, int>;

	//! Queue of pending messages awaiting display
	QVector<ConsoleItemType> m_queue;

	//! Auto-refresh timer
	QTimer m_timer;

	//! Log file handle
	QFile m_logFile;
	//! Text stream for log file output
	QTextStream* m_logStream;

	//! Whether Qt message forwarding is enabled
	static bool s_showQtMessagesInConsole;
	//! Whether to redirect messages to stdout/stderr
	static bool s_redirectToStdOut;
};
