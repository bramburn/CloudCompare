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
 * @file ccConsole.cpp
 *
 * @brief Console widget implementation for CloudCompare.
 *
 * @details Implements the ccConsole class which provides centralized logging
 * and message display functionality. This file contains:
 * - Singleton console instance management
 * - Qt message handler integration
 * - Thread-safe message queuing and display
 * - File-based logging support
 * - Error dialog integration
 *
 * @see ccConsole
 * @see ccLog
 */

#include "ccConsole.h"

// Local
#include "ccPersistentSettings.h"
#include "mainwindow.h"

// qCC_db
#include <ccSingleton.h>

// Qt
#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QThread>
#include <QTime>

// system
#include <cassert>
#ifdef QT_DEBUG
#include <iostream>
#endif

/***************
 *** Globals ***
 ***************/

// unique console instance
static ccSingleton<ccConsole> s_console;

bool       ccConsole::s_showQtMessagesInConsole = false;
bool       ccConsole::s_redirectToStdOut        = false;
static int s_refreshCycle_ms                    = 1000;

/*** ccCustomQListWidget implementation ***/

/**
 * @brief Constructor for ccCustomQListWidget.
 *
 * @param[in] parent Parent QWidget (optional).
 */
ccCustomQListWidget::ccCustomQListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

/**
 * @brief Handles key press events for clipboard copy operations.
 *
 * @param[in,out] event The key event to process.
 *
 * @details When the user presses a copy keyboard shortcut (Ctrl+C),
 * all currently selected items are copied to the system clipboard
 * as newline-separated text.
 */
void ccCustomQListWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->matches(QKeySequence::Copy))
	{
		int         itemsCount = count();
		QStringList strings;
		for (int i = 0; i < itemsCount; ++i)
		{
			if (item(i)->isSelected())
			{
				strings << item(i)->text();
			}
		}

		QApplication::clipboard()->setText(strings.join("\n"));
	}
	else
	{
		QListWidget::keyPressEvent(event);
	}
}

/**
 * @brief Set the console refresh cycle interval.
 *
 * @param[in] cycle_ms Refresh interval in milliseconds. Must be positive.
 *
 * @note If the console is currently in auto-refresh mode, this
 * method restarts the timer with the new interval.
 */
void ccConsole::SetRefreshCycle(int cycle_ms /*=1000*/)
{
	if (cycle_ms <= 0)
	{
		// invalid
		Warning("Invalid refresh cycle (can't be zero of negative)");
		return;
	}

	if (cycle_ms != s_refreshCycle_ms)
	{
		s_refreshCycle_ms = cycle_ms;

		if (s_console.instance && s_console.instance->autoRefresh())
		{
			// force the internal timer update
			s_console.instance->setAutoRefresh(false);
			s_console.instance->setAutoRefresh(true);
		}
	}
}

/**
 * @brief Get the singleton console instance.
 *
 * @param[in] autoInit If true and no instance exists, creates a minimal
 *                    temporary instance.
 * @return Pointer to the console instance, or nullptr if not initialized.
 */
ccConsole* ccConsole::TheInstance(bool autoInit /*=true*/)
{
	if (!s_console.instance && autoInit)
	{
		s_console.instance = new ccConsole;
		ccLog::RegisterInstance(s_console.instance);
	}

	return s_console.instance;
}

/**
 * @brief Release and destroy the console singleton.
 *
 * @param[in] flush If true, flushes any pending messages first.
 */
void ccConsole::ReleaseInstance(bool flush /*=true*/)
{
	if (flush && s_console.instance)
	{
		// DGM: just in case some messages are still in the queue
		s_console.instance->refresh();
	}
	ccLog::RegisterInstance(nullptr);
	s_console.release();
}

/**
 * @brief Default constructor.
 */
ccConsole::ccConsole()
    : m_textDisplay(nullptr)
    , m_parentWidget(nullptr)
    , m_parentWindow(nullptr)
    , m_logStream(nullptr)
{
}

/**
 * @brief Destructor.
 *
 * @details Closes any open log file and releases resources.
 */
ccConsole::~ccConsole()
{
	setLogFile(QString()); // to close/delete any active stream
}

/**
 * @brief Custom Qt message handler.
 *
 * @param[in] type Message severity level (QtDebugMsg, QtWarningMsg, etc.).
 * @param[in] context Qt message log context (file, line, function).
 * @param[in] msg The message text.
 *
 * @details This function intercepts Qt's internal messages (qDebug,
 * qWarning, qCritical, qFatal) and forwards them to the CloudCompare
 * logging system if Qt message forwarding is enabled.
 *
 * In debug mode, messages are also printed to stdout/stderr for
 * visibility in development environments.
 */
static void MyMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
#ifndef QT_DEBUG
	if (!ccConsole::QtMessagesEnabled())
	{
		return;
	}

	if (type == QtDebugMsg)
	{
		return;
	}
#endif

	QString message = QString("[%1] ").arg(context.function) + msg; // QString("%1 (%1:%1, %1)").arg(msg).arg(context.file).arg(context.line).arg(context.function);

	// in this function, you can write the message to any stream!
	switch (type)
	{
	case QtDebugMsg:
		ccLog::PrintDebug(msg);
		break;
	case QtWarningMsg:
		message.prepend("[Qt WARNING] ");
		ccLog::Warning(message);
		break;
	case QtCriticalMsg:
		message.prepend("[Qt CRITICAL] ");
		ccLog::Warning(message);
		break;
	case QtFatalMsg:
		message.prepend("[Qt FATAL] ");
		ccLog::Warning(message);
		break;
	case QtInfoMsg:
		message.prepend("[Qt INFO] ");
		ccLog::Warning(message);
		break;
	}

#ifdef QT_DEBUG
	// Also send the message to the console so we can look at the output when CC has quit
	//	(in Qt Creator's Application Output for example)
	switch (type)
	{
	case QtDebugMsg:
	case QtWarningMsg:
	case QtInfoMsg:
		std::cout << message.toStdString() << std::endl;
		break;

	case QtCriticalMsg:
	case QtFatalMsg:
		std::cerr << message.toStdString() << std::endl;
		break;
	}

#endif
}

/**
 * @brief Enable or disable forwarding of Qt messages to the console.
 *
 * @param[in] state true to enable Qt message forwarding, false to disable.
 *
 * @details When enabled, Qt's internal messages (qDebug, qWarning,
 * qCritical, qFatal) are forwarded to the ccConsole logging system.
 *
 * @note The setting is persisted to application settings.
 */
void ccConsole::EnableQtMessages(bool state)
{
	s_showQtMessagesInConsole = state;

	// save to persistent settings
	QSettings settings;
	settings.beginGroup(ccPS::Console());
	settings.setValue("QtMessagesEnabled", s_showQtMessagesInConsole);
	settings.endGroup();
}

void ccConsole::Init(QListWidget* textDisplay /*=nullptr*/,
                     QWidget*     parentWidget /*=nullptr*/,
                     MainWindow*  parentWindow /*=nullptr*/,
                     bool         redirectToStdOut /*=false*/)
{
	// should be called only once!
	if (s_console.instance)
	{
		assert(false);
		return;
	}

	s_console.instance                 = new ccConsole;
	s_console.instance->m_textDisplay  = textDisplay;
	s_console.instance->m_parentWidget = parentWidget;
	s_console.instance->m_parentWindow = parentWindow;
	s_redirectToStdOut                 = redirectToStdOut;

	if (s_redirectToStdOut)
	{
		// make the system console/terminal more responsive by removing any buffering
		setbuf(stdout, NULL);
	}

	// auto-start
	if (textDisplay)
	{
		// load from persistent settings
		QSettings settings;
		settings.beginGroup(ccPS::Console());
		s_showQtMessagesInConsole = settings.value("QtMessagesEnabled", false).toBool();
		settings.endGroup();

		// install : set the callback for Qt messages
		qInstallMessageHandler(MyMessageOutput);

		s_console.instance->setAutoRefresh(true);
	}
	ccLog::RegisterInstance(s_console.instance);
}

bool ccConsole::autoRefresh() const
{
	return m_timer.isActive();
}

/**
 * @brief Enable or disable automatic message refresh.
 *
 * @param[in] state true to enable auto-refresh, false to disable.
 *
 * @details When enabled, the refresh timer starts and messages are
 * automatically displayed at the interval set by SetRefreshCycle().
 */
void ccConsole::setAutoRefresh(bool state)
{
	if (state)
	{
		connect(&m_timer, &QTimer::timeout, this, &ccConsole::refresh);
		m_timer.start(s_refreshCycle_ms);
	}
	else
	{
		m_timer.stop();
		disconnect(&m_timer, &QTimer::timeout, this, &ccConsole::refresh);
	}
}

/**
 * @brief Refresh the console display by processing queued messages.
 *
 * @details Moves all pending messages from the internal queue to the
 * display widget and/or log file. Messages are color-coded based on
 * severity:
 * - Red: Error messages
 * - Magenta: Warning messages
 * - Blue (debug only): Debug messages
 *
 * @note This method is thread-safe and can be called from any thread.
 */
void ccConsole::refresh()
{
	m_mutex.lock();

	if (!m_queue.isEmpty())
	{
		if (m_textDisplay || m_logStream)
		{
			for (auto messagePair : m_queue)
			{
				// destination: log file
				if (m_logStream)
				{
					*m_logStream << messagePair.first << Qt::endl;
				}

				// destination: console widget
				if (m_textDisplay)
				{
					// messagePair.first = message text
					QListWidgetItem* item = new QListWidgetItem(messagePair.first);

					// set color based on the message severity
					if ((messagePair.second & LOG_ERROR) == LOG_ERROR) // Error
					{
						item->setForeground(Qt::red);
					}
					else if ((messagePair.second & LOG_WARNING) == LOG_WARNING) // Warning
					{
						item->setForeground(Qt::magenta);
						// we also force the console visibility if a warning message arrives!
						if (m_parentWindow)
						{
							m_parentWindow->forceConsoleDisplay();
						}
					}
#ifdef QT_DEBUG
					else if (messagePair.second & DEBUG_FLAG) // Debug
					{
						item->setForeground(Qt::blue);
					}
#endif

					m_textDisplay->addItem(item);
				}
			}

			if (m_logStream)
			{
				m_logFile.flush();
			}

			if (m_textDisplay)
			{
				m_textDisplay->scrollToBottom();
			}
		}

		m_queue.clear();
	}

	m_mutex.unlock();
}

/**
 * @brief Process and display a log message.
 *
 * @param[in] message The formatted message text to log.
 * @param[in] level Message severity level (see ccLog constants).
 *
 * @details Implementation of ccLog::logMessage().
 * - Messages are filtered by current verbosity level
 * - In debug mode without a display widget, messages go to stdout/stderr
 * - Error messages on the main thread trigger a QMessageBox dialog
 */
void ccConsole::logMessage(const QString& message, int level)
{
	// skip messages below the current 'verbosity' level
	if ((level & 7) < ccLog::VerbosityLevel())
	{
		return;
	}

	QString formatedMessage = QStringLiteral("[") + QTime::currentTime().toString() + QStringLiteral("] ") + message;
	if (s_redirectToStdOut)
	{
		printf("%s\n", qUtf8Printable(formatedMessage));
	}
	if (m_textDisplay || m_logStream)
	{
		m_mutex.lock();
		m_queue.push_back(ConsoleItemType(formatedMessage, level));
		m_mutex.unlock();
	}
#ifdef QT_DEBUG
	else if (!s_redirectToStdOut)
	{
		// Error
		if (level & LOG_ERROR)
		{
			if (level & DEBUG_FLAG)
				printf("ERR-DBG: ");
			else
				printf("ERR: ");
		}
		// Warning
		else if (level & LOG_WARNING)
		{
			if (level & DEBUG_FLAG)
				printf("WARN-DBG: ");
			else
				printf("WARN: ");
		}
		// Standard
		else
		{
			if (level & DEBUG_FLAG)
				printf("MSG-DBG: ");
			else
				printf("MSG: ");
		}
		printf(" %s\n", qUtf8Printable(formatedMessage));
	}
#endif

	// we display the error messages in a popup dialog
	if ((level & LOG_ERROR)
	    && qApp
	    && m_parentWidget
	    && QThread::currentThread() == qApp->thread())
	{
		QMessageBox::warning(m_parentWidget, "Error", message);
	}
}

/**
 * @brief Set or clear the log file for persistent message logging.
 *
 * @param[in] filename Path to log file. If empty, logging to file is disabled.
 * @return true if file was successfully opened/closed, false on error.
 *
 * @details If a file is already open, it is closed before opening the new one.
 * Setting an empty filename closes the current log file.
 * When a file is opened, auto-refresh is automatically enabled.
 */
bool ccConsole::setLogFile(const QString& filename)
{
	// close previous stream (if any)
	if (m_logStream)
	{
		m_mutex.lock();
		delete m_logStream;
		m_logStream = nullptr;
		m_mutex.unlock();

		if (m_logFile.isOpen())
		{
			m_logFile.close();
		}
	}

	if (!filename.isEmpty())
	{
		m_logFile.setFileName(filename);
		if (!m_logFile.open(QFile::Text | QFile::WriteOnly))
		{
			return Error(QString("[Console] Failed to open/create log file '%1'").arg(filename));
		}

		m_mutex.lock();
		m_logStream = new QTextStream(&m_logFile);
		m_mutex.unlock();
		setAutoRefresh(true);
	}

	return true;
}
