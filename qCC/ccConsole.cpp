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
 * @brief Console widget implementation
 *
 * Implements the logging console widget for CloudCompare with support for
 * text output, warning/error messages, clipboard operations, log file
 * persistence, and Qt message forwarding.
 *
 * @see ccConsole, ccLog
 */

// Local
#include "ccConsole.h"
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

//! Singleton instance holder
static ccSingleton<ccConsole> s_console;

//! Static: whether Qt message forwarding to console is enabled
bool ccConsole::s_showQtMessagesInConsole = false;

//! Static: whether to redirect log messages to stdout/stderr
bool ccConsole::s_redirectToStdOut = false;

//! Global refresh cycle in milliseconds
static int s_refreshCycle_ms = 1000;

/*** ccCustomQListWidget ***/

// ccCustomQListWidget::ccCustomQListWidget
ccCustomQListWidget::ccCustomQListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

// ccCustomQListWidget::keyPressEvent
void ccCustomQListWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->matches(QKeySequence::Copy))
	{
		// Collect all selected item texts
		int         itemsCount = count();
		QStringList strings;
		strings.reserve(itemsCount);
		for (int i = 0; i < itemsCount; ++i)
		{
			if (item(i)->isSelected())
			{
				strings << item(i)->text();
			}
		}

		// Copy joined strings to the system clipboard
		QApplication::clipboard()->setText(strings.join("\n"));
	}
	else
	{
		QListWidget::keyPressEvent(event);
	}
}

// ccConsole::SetRefreshCycle
void ccConsole::SetRefreshCycle(int cycle_ms)
{
	if (cycle_ms <= 0)
	{
		Warning("Invalid refresh cycle (can't be zero or negative)");
		return;
	}

	if (cycle_ms != s_refreshCycle_ms)
	{
		s_refreshCycle_ms = cycle_ms;

		// Force the internal timer to pick up the new interval
		if (s_console.instance && s_console.instance->autoRefresh())
		{
			s_console.instance->setAutoRefresh(false);
			s_console.instance->setAutoRefresh(true);
		}
	}
}

// ccConsole::TheInstance
ccConsole* ccConsole::TheInstance(bool autoInit)
{
	if (!s_console.instance && autoInit)
	{
		s_console.instance = new ccConsole;
		ccLog::RegisterInstance(s_console.instance);
	}

	return s_console.instance;
}

// ccConsole::ReleaseInstance
void ccConsole::ReleaseInstance(bool flush)
{
	if (flush && s_console.instance)
	{
		// Flush any messages still in the queue
		s_console.instance->refresh();
	}
	ccLog::RegisterInstance(nullptr);
	s_console.release();
}

// ccConsole::ccConsole
ccConsole::ccConsole()
    : m_textDisplay(nullptr)
    , m_parentWidget(nullptr)
    , m_parentWindow(nullptr)
    , m_logStream(nullptr)
{
}

// ccConsole::~ccConsole
ccConsole::~ccConsole()
{
	setLogFile(QString()); // Closes any open log file
}

//! Qt message handler callback
/**
 * Intercepts Qt's qDebug, qWarning, qCritical, qFatal, and qInfo messages
 * and routes them through the ccLog system.
 *
 * In release builds, only messages are shown if EnableQtMessages(true) has
 * been called. QtDebugMsg is always filtered out in release builds.
 * In debug builds, messages are also echoed to std::cout/std::cerr.
 *
 * @param type    Message severity level
 * @param context Qt message log context (file, line, function)
 * @param msg     Message text
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

	// Prefix with the originating function name
	QString message = QString("[%1] ").arg(context.function) + msg;

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
	// Echo to system console in debug mode
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

// ccConsole::EnableQtMessages
void ccConsole::EnableQtMessages(bool state)
{
	s_showQtMessagesInConsole = state;

	// Persist the setting
	QSettings settings;
	settings.beginGroup(ccPS::Console());
	settings.setValue("QtMessagesEnabled", s_showQtMessagesInConsole);
	settings.endGroup();
}

// ccConsole::Init
void ccConsole::Init(QListWidget* textDisplay, QWidget* parentWidget, MainWindow* parentWindow, bool redirectToStdOut)
{
	// Init should only be called once
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
		// Remove stdout buffering for more responsive terminal output
		setbuf(stdout, NULL);
	}

	if (textDisplay)
	{
		// Restore Qt message forwarding setting from persistent storage
		QSettings settings;
		settings.beginGroup(ccPS::Console());
		s_showQtMessagesInConsole = settings.value("QtMessagesEnabled", false).toBool();
		settings.endGroup();

		// Install the Qt message handler
		qInstallMessageHandler(MyMessageOutput);

		s_console.instance->setAutoRefresh(true);
	}

	ccLog::RegisterInstance(s_console.instance);
}

// ccConsole::autoRefresh
bool ccConsole::autoRefresh() const
{
	return m_timer.isActive();
}

// ccConsole::setAutoRefresh
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

// ccConsole::refresh
void ccConsole::refresh()
{
	m_mutex.lock();

	if (!m_queue.isEmpty())
	{
		if (m_textDisplay || m_logStream)
		{
			for (const auto& messagePair : m_queue)
			{
				// Write to log file
				if (m_logStream)
				{
					*m_logStream << messagePair.first << Qt::endl;
				}

				// Add to on-screen display
				if (m_textDisplay)
				{
					QListWidgetItem* item = new QListWidgetItem(messagePair.first);

					// Color-code based on severity
					if ((messagePair.second & LOG_ERROR) == LOG_ERROR)
					{
						item->setForeground(Qt::red);
					}
					else if ((messagePair.second & LOG_WARNING) == LOG_WARNING)
					{
						item->setForeground(Qt::magenta);
						// Force the console pane to become visible when a warning arrives
						if (m_parentWindow)
						{
							m_parentWindow->forceConsoleDisplay();
						}
					}
#ifdef QT_DEBUG
					else if (messagePair.second & DEBUG_FLAG)
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

// ccConsole::logMessage
void ccConsole::logMessage(const QString& message, int level)
{
	// Skip messages below the current verbosity threshold
	if ((level & 7) < ccLog::VerbosityLevel())
	{
		return;
	}

	// Prepend timestamp
	QString formattedMessage = QStringLiteral("[") + QTime::currentTime().toString() + QStringLiteral("] ") + message;

	if (s_redirectToStdOut)
	{
		printf("%s\n", qUtf8Printable(formattedMessage));
	}

	if (m_textDisplay || m_logStream)
	{
		m_mutex.lock();
		m_queue.push_back(ConsoleItemType(formattedMessage, level));
		m_mutex.unlock();
	}
#ifdef QT_DEBUG
	else if (!s_redirectToStdOut)
	{
		// Fallback: print directly to stdout/stderr in debug mode
		// when no display or log file is configured
		if (level & LOG_ERROR)
		{
			printf("%s: ", (level & DEBUG_FLAG) ? "ERR-DBG" : "ERR");
		}
		else if (level & LOG_WARNING)
		{
			printf("%s: ", (level & DEBUG_FLAG) ? "WARN-DBG" : "WARN");
		}
		else
		{
			printf("%s: ", (level & DEBUG_FLAG) ? "MSG-DBG" : "MSG");
		}
		printf(" %s\n", qUtf8Printable(formattedMessage));
	}
#endif

	// Show error messages in a blocking dialog if we have a parent widget
	// and we're on the main thread
	if ((level & LOG_ERROR) && qApp && m_parentWidget && QThread::currentThread() == qApp->thread())
	{
		QMessageBox::warning(m_parentWidget, "Error", message);
	}
}

// ccConsole::setLogFile
bool ccConsole::setLogFile(const QString& filename)
{
	// Close any previously open log file
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
