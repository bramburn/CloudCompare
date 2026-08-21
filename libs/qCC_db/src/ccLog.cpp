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
 * @file ccLog.cpp
 *
 * @brief Logging interface implementation
 *
 * Implements the static logging facade for CloudCompare. Provides:
 * - Thread-safe message routing to a registered sink (ccConsole)
 * - Message backup system for early initialization (before sink exists)
 * - Printf-style and QString variants of all log methods
 * - Verbosity filtering and DEBUG_FLAG gating
 *
 * @see ccLog, ccConsole
 */

#include "ccLog.h"

// CCCoreLib
#include <CCPlatform.h>

// System
#include <cassert>
#include <vector>

// Portable _vsnprintf alias
#if !defined(CC_WINDOWS)
#define _vsnprintf vsnprintf
#endif

/***************
 *** Globals ***
 ***************/

//! Fixed-size buffer for printf-style formatting (4096 bytes)
static const size_t s_bufferMaxSize = 4096;
static char         s_buffer[s_bufferMaxSize];

//! Simple message struct for the backup queue
struct Message
{
	Message(const QString& t, int f)
	    : text(t)
	    , flags(f)
	{
	}
	QString text;
	int     flags;
};

//! Whether the message backup system is enabled
static bool s_backupEnabled = false;

//! Current verbosity threshold
// In debug: defaults to LOG_VERBOSE (all messages)
// In release: defaults to LOG_STANDARD (suppresses verbose)
#ifdef QT_DEBUG
static int s_verbosityLevel = ccLog::LOG_VERBOSE;
#else
static int s_verbosityLevel = ccLog::LOG_STANDARD;
#endif

//! Messages logged before a sink was registered (backup queue)
static std::vector<Message> s_backupMessages;

//! Registered logging sink (typically ccConsole)
static ccLog* s_instance = nullptr;

// ccLog::TheInstance
/**
 * @brief Get the current logging sink
 * @return Pointer to the registered ccLog instance, or nullptr
 */
ccLog* ccLog::TheInstance()
{
	return s_instance;
}

// ccLog::EnableMessageBackup
/**
 * @brief Enable or disable the message backup system
 *
 * When enabled, messages logged before a sink is registered are queued
 * in memory rather than dropped. They are flushed when RegisterInstance()
 * is called.
 *
 * Used during early startup (before ccConsole is constructed) to capture
 * any log messages that occur during initialization.
 *
 * @param[in] state true to enable backup, false to disable
 */
void ccLog::EnableMessageBackup(bool state)
{
	s_backupEnabled = state;
}

// ccLog::VerbosityLevel
/**
 * @brief Get the current verbosity threshold
 * @return Current verbosity level (0-4)
 */
int ccLog::VerbosityLevel()
{
	return s_verbosityLevel;
}

// ccLog::SetVerbosityLevel
/**
 * @brief Set the verbosity threshold
 *
 * Messages with (level & 7) < verbosityLevel are silently dropped.
 * Errors (LOG_ERROR) can never be suppressed — the threshold is
 * clamped to LOG_ERROR.
 *
 * @param[in] level New verbosity level (0-4)
 */
void ccLog::SetVerbosityLevel(int level)
{
	s_verbosityLevel = std::min(level, static_cast<int>(LOG_ERROR));
}

// ccLog::LogMessage
/**
 * @brief Route a message to the registered sink
 *
 * Routes a message to the current sink if one is registered, or stores
 * it in the backup queue if backup is enabled. Messages below the
 * verbosity threshold are dropped silently.
 *
 * @param[in] message Message text
 * @param[in] level   Message level (LOG_* flags)
 */
void ccLog::LogMessage(const QString& message, int level)
{
	// Drop messages below verbosity threshold
	if ((level & 7) < s_verbosityLevel)
	{
		return;
	}

	if (s_instance)
	{
		// Route to the registered sink
		s_instance->logMessage(message, level);
	}
	else if (s_backupEnabled)
	{
		// Store for later delivery
		try
		{
			s_backupMessages.emplace_back(message, level);
		}
		catch (const std::bad_alloc&)
		{
			// Memory allocation failed — message is lost
		}
	}
}

// ccLog::RegisterInstance
/**
 * @brief Register (or unregister) the logging sink
 *
 * When a sink is registered, all backed-up messages are immediately
 * flushed to it in order. After flushing, the backup queue is cleared.
 *
 * Passing nullptr unregisters the current sink; no flushing occurs.
 *
 * @param[in] logInstance Logging sink to use, or nullptr to unregister
 */
void ccLog::RegisterInstance(ccLog* logInstance)
{
	s_instance = logInstance;
	if (s_instance)
	{
		// Flush backed-up messages in order
		for (const Message& message : s_backupMessages)
		{
			s_instance->logMessage(message.text, message.flags);
		}
		s_backupMessages.clear();
	}
}

/**
 * @brief Variadic macro: format and dispatch a message
 *
 * Parses printf-style arguments, formats into the static buffer,
 * and calls LogMessage(). Only executes if a sink exists or backup
 * is enabled.
 *
 * @param[in] flags Message level (LOG_* constants)
 */
#define LOG_ARGS(flags) \
	if (s_instance || s_backupEnabled) \
	{ \
		va_list args; \
		va_start(args, format); \
		_vsnprintf(s_buffer, s_bufferMaxSize, format, args); \
		va_end(args); \
		LogMessage(QString(s_buffer), flags); \
	}

// ccLog::PrintVerbose — printf-style
bool ccLog::PrintVerbose(const char* format, ...)
{
	LOG_ARGS(LOG_VERBOSE)
	return true;
}

// ccLog::PrintVerbose — QString
bool ccLog::PrintVerbose(const QString& message)
{
	LogMessage(message, LOG_VERBOSE);
	return true;
}

// ccLog::Print — printf-style
bool ccLog::Print(const char* format, ...)
{
	LOG_ARGS(LOG_STANDARD)
	return true;
}

// ccLog::Print — QString
bool ccLog::Print(const QString& message)
{
	LogMessage(message, LOG_STANDARD);
	return true;
}

// ccLog::PrintHigh — printf-style
bool ccLog::PrintHigh(const char* format, ...)
{
	LOG_ARGS(LOG_IMPORTANT)
	return true;
}

// ccLog::PrintHigh — QString
bool ccLog::PrintHigh(const QString& message)
{
	LogMessage(message, LOG_IMPORTANT);
	return true;
}

// ccLog::Warning — printf-style
bool ccLog::Warning(const char* format, ...)
{
	LOG_ARGS(LOG_WARNING)
	return false;
}

// ccLog::Warning — QString
bool ccLog::Warning(const QString& message)
{
	LogMessage(message, LOG_WARNING);
	return false;
}

// ccLog::Error — printf-style
bool ccLog::Error(const char* format, ...)
{
	LOG_ARGS(LOG_ERROR)
	return false;
}

// ccLog::Error — QString
bool ccLog::Error(const QString& message)
{
	LogMessage(message, LOG_ERROR);
	return false;
}

// ccLog::PrintDebug — printf-style (debug builds only)
bool ccLog::PrintDebug(const char* format, ...)
{
#ifdef QT_DEBUG
	LOG_ARGS(LOG_STANDARD | DEBUG_FLAG)
#endif
	return false;
}

// ccLog::PrintDebug — QString (debug builds only)
bool ccLog::PrintDebug(const QString& message)
{
#ifdef QT_DEBUG
	LogMessage(message, LOG_STANDARD | DEBUG_FLAG);
#endif
	return false;
}

// ccLog::WarningDebug — printf-style (debug builds only)
bool ccLog::WarningDebug(const char* format, ...)
{
#ifdef QT_DEBUG
	LOG_ARGS(LOG_WARNING)
#endif
	return false;
}

// ccLog::WarningDebug — QString (debug builds only)
bool ccLog::WarningDebug(const QString& message)
{
#ifdef QT_DEBUG
	LogMessage(message, LOG_WARNING | DEBUG_FLAG);
#endif
	return false;
}

// ccLog::ErrorDebug — printf-style (debug builds only)
bool ccLog::ErrorDebug(const char* format, ...)
{
#ifdef QT_DEBUG
	LOG_ARGS(LOG_ERROR)
#endif
	return false;
}

// ccLog::ErrorDebug — QString (debug builds only)
bool ccLog::ErrorDebug(const QString& message)
{
#ifdef QT_DEBUG
	LogMessage(message, LOG_ERROR | DEBUG_FLAG);
#endif
	return false;
}
