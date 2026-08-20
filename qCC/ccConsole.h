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
 * @brief Console widget and logging infrastructure for CloudCompare.
 *
 * @details The ccConsole class provides a centralized logging and message
 * display system for the CloudCompare application. It handles:
 * - Display of informational, warning, and error messages in a GUI list widget
 * - Thread-safe message queuing with automatic refresh
 * - Optional logging to file
 * - Qt message handler integration (qDebug, qWarning, etc.)
 * - Automatic popup dialogs for critical errors
 *
 * @section usage Usage
 *
 * The console is typically initialized at application startup:
 * @code
 * ccConsole::Init(ui.consoleWidget);  // Attach to GUI list widget
 * ccLog::Print("Application started");
 * ccLog::Warning("Something might be wrong");
 * ccLog::Error("Critical failure!");
 * @endcode
 *
 * @section threading Thread Safety
 *
 * This class is reentrant but not fully thread-safe for concurrent calls
 * to logMessage. Use the ccLog static methods from any thread - they
 * internally handle synchronization.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see ccLog
 * @see MainWindow::ccConsole
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
 * @brief Custom QListWidget with enhanced copy functionality.
 *
 * @details Extends QListWidget to provide convenient keyboard-based
 * copy operations. When the user presses Ctrl+C (or another copy
 * shortcut), all selected items are copied to the clipboard as
 * newline-separated text.
 *
 * @note Currently only implements copy; paste is not supported.
 *
 * @extends QListWidget
 */
class ccCustomQListWidget : public QListWidget
{
	Q_OBJECT

  public:
	ccCustomQListWidget(QWidget* parent = nullptr);

  protected:
	void keyPressEvent(QKeyEvent* event) override;
};

/**
 * @brief Central console/logging interface for CloudCompare.
 *
 * @details The ccConsole class serves as the primary logging and message
 * display system. It inherits from ccLog to receive log messages and
 * displays them in an associated QListWidget.
 *
 * Key features:
 * - Singleton pattern: use TheInstance() to access
 * - Message queuing with automatic refresh timer
 * - Thread-safe logging from multiple threads
 * - Optional file logging
 * - Automatic error dialogs
 * - Integration with Qt's message handling system
 *
 * @section modes Operating Modes
 *
 * - Debug mode: All messages are also sent to stdout/stderr
 * - Release mode: Only messages with attached widget are displayed
 * - Error handling: Critical errors show blocking QMessageBox dialogs
 *
 * @extends QObject
 * @extends ccLog
 * @nosubgrouping
 */
class ccConsole : public QObject
    , public ccLog
{
	Q_OBJECT

  public:
	/**
	 * @brief Destructor.
	 *
	 * Cleans up resources including closing any open log files
	 * and stopping the refresh timer.
	 */
	~ccConsole() override;

	/**
	 * @brief Initialize the console singleton.
	 *
	 * @details Sets up the console with optional widget attachments.
	 * This method should be called exactly once during application startup.
	 *
	 * @warning In release mode, no messages are output unless a valid
	 * 'textDisplay' widget is provided. Error messages only appear as
	 * blocking QMessageBox dialogs if a 'parentWidget' is specified.
	 *
	 * @warning In debug mode, all messages are also sent to the system
	 * console (stdout/stderr via printf).
	 *
	 * @param[in] textDisplay Optional QListWidget for displaying messages.
	 *                        If nullptr, messages are queued but not displayed
	 *                        in the GUI (useful for headless/CLI mode).
	 * @param[in] parentWidget Optional parent widget for error message dialogs.
	 *                         If nullptr, error dialogs are not shown.
	 * @param[in] parentWindow Optional reference to MainWindow for forcing
	 *                         console visibility when warnings arrive.
	 * @param[in] redirectToStdOut If true, messages are also printed to stdout.
	 *                              Useful for CLI mode with redirected output.
	 */
	static void Init(QListWidget* textDisplay      = nullptr,
	                 QWidget*     parentWidget     = nullptr,
	                 MainWindow*  parentWindow     = nullptr,
	                 bool         redirectToStdOut = false);

	/**
	 * @brief Get the singleton console instance.
	 *
	 * @param[in] autoInit If true and no instance exists, creates a temporary
	 *                    instance with no attached widgets.
	 * @return Pointer to the console instance, or nullptr if not initialized
	 *         and autoInit is false.
	 */
	static ccConsole* TheInstance(bool autoInit = true);

	/**
	 * @brief Release and destroy the console singleton.
	 *
	 * @param[in] flush If true (default), flushes any pending messages
	 *                  before destroying the instance.
	 */
	static void ReleaseInstance(bool flush = true);

	/**
	 * @brief Set the console refresh cycle interval.
	 *
	 * @param[in] cycle_ms Refresh interval in milliseconds. Must be positive.
	 *                    Default is 1000ms (1 second).
	 *
	 * @warning If cycle_ms <= 0, the call is ignored and a warning is logged.
	 */
	static void SetRefreshCycle(int cycle_ms = 1000);

	/**
	 * @brief Enable or disable automatic message refresh.
	 *
	 * @param[in] state true to enable auto-refresh, false to disable.
	 *
	 * @see autoRefresh()
	 */
	void setAutoRefresh(bool state);

	/**
	 * @brief Check if auto-refresh is currently active.
	 * @return true if auto-refresh timer is running, false otherwise.
	 *
	 * @see setAutoRefresh()
	 */
	bool autoRefresh() const;

	/**
	 * @brief Set or clear the log file for persistent message logging.
	 *
	 * @param[in] filename Path to log file. If empty, logging to file is disabled.
	 * @return true if file was successfully opened/closed, false on error.
	 */
	bool setLogFile(const QString& filename);

	/**
	 * @brief Enable or disable forwarding of Qt messages to the console.
	 *
	 * @param[in] state true to enable Qt message forwarding, false to disable.
	 *
	 * @note Setting is persisted to application settings.
	 */
	static void EnableQtMessages(bool state);

	/**
	 * @brief Check if Qt message forwarding is enabled.
	 * @return true if Qt messages are being forwarded to the console.
	 */
	static bool QtMessagesEnabled()
	{
		return s_showQtMessagesInConsole;
	}

	/**
	 * @brief Get the parent widget used for error dialogs.
	 * @return Pointer to the parent widget, or nullptr if not set.
	 */
	inline QWidget* parentWidget()
	{
		return m_parentWidget;
	}

  public:
	/**
	 * @brief Refresh the console display by processing queued messages.
	 *
	 * Moves all pending messages from the queue to the display
	 * widget and/or log file. This method is typically called automatically
	 * by the refresh timer, but can be called manually to force immediate display.
	 */
	void refresh();

  protected:
	/**
	 * @brief Default constructor.
	 *
	 * @warning Constructor is protected to enforce singleton usage.
	 */
	ccConsole();

	/**
	 * @brief Process and display a log message.
	 *
	 * @param[in] message The message text to log.
	 * @param[in] level Message severity level (see ccLog constants).
	 *
	 * @extends ccLog::logMessage()
	 */
	void logMessage(const QString& message, int level) override;

	/** @brief Associated text display widget for message output. */
	QListWidget* m_textDisplay;

	/** @brief Parent widget for error message dialogs. */
	QWidget* m_parentWidget;

	/** @brief Reference to the main window. */
	MainWindow* m_parentWindow;

	/** @brief Mutex for thread-safe message queue access. */
	QMutex m_mutex;

	/** @brief Type for queued message items (message text + severity level). */
	using ConsoleItemType = QPair<QString, int>;

	/** @brief Queue of messages awaiting display. */
	QVector<ConsoleItemType> m_queue;

	/** @brief Timer for automatic refresh. */
	QTimer m_timer;

	/** @brief Log file for persistent message logging. */
	QFile m_logFile;

	/** @brief Stream for writing to the log file. */
	QTextStream* m_logStream;

	/** @brief Whether to forward Qt messages to the console. */
	static bool s_showQtMessagesInConsole;

	/** @brief Whether to also print messages to stdout. */
	static bool s_redirectToStdOut;
};
