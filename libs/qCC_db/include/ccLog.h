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
 * @file ccLog.h
 *
 * @brief Logging interface for CloudCompare
 *
 * Provides a thread-safe, static-facade logging interface used throughout
 * CloudCompare for console output, warnings, and error messages.
 *
 * Usage:
 * @code
 * ccLog::Print("Loading file: %s", filename);
 * ccLog::Warning("Low memory");
 * ccLog::Error("Failed to open: %s", filename);
 * @endcode
 *
 * Architecture:
 * - This class is a **static facade** — all methods are static
 * - Actual logging is delegated to a **registered sink** (ccConsole)
 * - Before a sink is registered, messages can be **backed up** in memory
 * - All public methods are **thread-safe**
 *
 * Thread Safety:
 * The facade itself (LogMessage, RegisterInstance) is thread-safe via
 * the static global variables. Concrete sink implementations (e.g.,
 * ccConsole) must also be thread-safe.
 *
 * Message Levels (lowest to highest):
 * LOG_VERBOSE (0) → LOG_STANDARD (1) → LOG_IMPORTANT (2) →
 * LOG_WARNING (3) → LOG_ERROR (4)
 * The DEBUG_FLAG (8) can be OR'd with any level to mark debug-only messages.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see ccConsole
 */

// Local
#include "qCC_db.h"

// system
#include <stdio.h>
#include <string>

// Qt
#include <QString>

/**
 * @brief Static logging facade
 *
 * Thread-safe logging interface used as a singleton facade.
 * All methods are static; actual logging is delegated to a registered
 * sink instance (typically ccConsole). Before a sink is registered,
 * messages can optionally be backed up in memory for later delivery.
 *
 * Level hierarchy (0-4, plus DEBUG_FLAG):
 * - 0 LOG_VERBOSE  : Debug/trace messages (hidden in release by default)
 * - 1 LOG_STANDARD  : Normal informational messages
 * - 2 LOG_IMPORTANT : Highlighted/high-priority messages
 * - 3 LOG_WARNING   : Warning messages
 * - 4 LOG_ERROR     : Error messages (never suppressed)
 * - +8 DEBUG_FLAG   : Debug-only message marker (stripped in release)
 *
 * @note Use the convenience macros (ccLog::Print, ccLog::Warning, etc.)
 *       rather than calling logMessage() directly.
 */
class QCC_DB_LIB_API ccLog
{
  public:
	/**
	 * @brief Destructor
	 */
	virtual ~ccLog()
	{
	}

	/**
	 * @brief Get the current logging sink
	 * @return Pointer to the registered sink, or nullptr
	 */
	static ccLog* TheInstance();

	/**
	 * @brief Register (or unregister) the logging sink
	 *
	 * When a sink is registered, any backed-up messages are flushed to it.
	 * Passing nullptr unregisters the current sink.
	 *
	 * @param[in] logInstance Sink to use, or nullptr to unregister
	 */
	static void RegisterInstance(ccLog* logInstance);

	/**
	 * @brief Enable or disable message backup
	 *
	 * When enabled, messages logged before a sink is registered are queued
	 * in memory and flushed when RegisterInstance() is called. Used during
	 * early startup.
	 *
	 * @param[in] state true to enable backup, false to disable
	 */
	static void EnableMessageBackup(bool state);

	/**
	 * @brief Message severity levels
	 *
	 * Levels 0-4 form an increasing severity hierarchy. Messages with
	 * (level & 7) < VerbosityLevel() are silently dropped.
	 * The DEBUG_FLAG (8) can be OR'd with any level.
	 *
	 * In debug builds, verbosity defaults to LOG_VERBOSE (all messages).
	 * In release builds, it defaults to LOG_STANDARD (verbose suppressed).
	 */
	enum MessageLevelFlags
	{
		LOG_VERBOSE = 0,   //!< Debug/trace messages
		LOG_STANDARD = 1,  //!< Normal informational messages
		LOG_IMPORTANT = 2, //!< Highlighted messages
		LOG_WARNING = 3,   //!< Warning messages
		LOG_ERROR = 4,     //!< Error messages (never suppressed)

		DEBUG_FLAG = 8 //!< Debug-only message flag (stripped in release)
	};

	/**
	 * @brief Get the verbosity threshold
	 * @return Current verbosity level (0-4)
	 */
	static int VerbosityLevel();

	/**
	 * @brief Set the verbosity threshold
	 *
	 * Messages with (level & 7) < threshold are dropped.
	 * Errors (LOG_ERROR) can never be suppressed.
	 *
	 * @param[in] level New threshold (0-4)
	 */
	static void SetVerbosityLevel(int level);

	/**
	 * @brief Route a message to the registered sink
	 *
	 * Called by the static facade methods. Routes to the sink if one
	 * is registered, or stores in the backup queue if enabled.
	 *
	 * @param[in] message Message text
	 * @param[in] level   Message severity (see MessageLevelFlags)
	 */
	static void LogMessage(const QString& message, int level);

	/**
	 * @brief Core logging method — must be implemented by sinks
	 *
	 * Pure virtual: concrete sinks (ccConsole) implement this to actually
	 * display or store messages. Must be thread-safe.
	 *
	 * @param[in] message Message text
	 * @param[in] level   Message severity (see MessageLevelFlags)
	 */
	virtual void logMessage(const QString& message, int level) = 0;

	/**
	 * @brief Print a verbose/debug message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool PrintVerbose(const char* format, ...);

	/**
	 * @brief Print a verbose/debug message (QString)
	 * @param[in] message Message text
	 * @return Always true
	 */
	static bool PrintVerbose(const QString& message);

	/**
	 * @brief Print a standard informational message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool Print(const char* format, ...);

	/**
	 * @brief Print a standard informational message (QString)
	 * @param[in] message Message text
	 * @return Always true
	 */
	static bool Print(const QString& message);

	/**
	 * @brief Print a highlighted/important message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool PrintHigh(const char* format, ...);

	/**
	 * @brief Print a highlighted/important message (QString)
	 * @param[in] message Message text
	 * @return Always true
	 */
	static bool PrintHigh(const QString& message);

	/**
	 * @brief Print a debug-only message (printf-style)
	 *
	 * Compiled out entirely in release builds (QT_DEBUG not defined).
	 *
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining)
	 */
	static bool PrintDebug(const char* format, ...);

	/**
	 * @brief Print a debug-only message (QString)
	 *
	 * Compiled out entirely in release builds.
	 *
	 * @param[in] message Message text
	 * @return Always false
	 */
	static bool PrintDebug(const QString& message);

	/**
	 * @brief Print a warning (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining in conditional expressions)
	 */
	static bool Warning(const char* format, ...);

	/**
	 * @brief Print a warning (QString)
	 * @param[in] message Warning text
	 * @return Always false
	 */
	static bool Warning(const QString& message);

	/**
	 * @brief Print a debug-only warning (printf-style)
	 *
	 * Compiled out in release builds.
	 *
	 * @param[in] format Printf-style format string
	 * @return Always false
	 */
	static bool WarningDebug(const char* format, ...);

	/**
	 * @brief Print a debug-only warning (QString)
	 *
	 * Compiled out in release builds.
	 *
	 * @param[in] message Warning text
	 * @return Always false
	 */
	static bool WarningDebug(const QString& message);

	/**
	 * @brief Print an error (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining)
	 */
	static bool Error(const char* format, ...);

	/**
	 * @brief Print an error (QString)
	 * @param[in] message Error text
	 * @return Always false
	 */
	static bool Error(const QString& message);

	/**
	 * @brief Print a debug-only error (printf-style)
	 *
	 * Compiled out in release builds.
	 *
	 * @param[in] format Printf-style format string
	 * @return Always false
	 */
	static bool ErrorDebug(const char* format, ...);

	/**
	 * @brief Print a debug-only error (QString)
	 *
	 * Compiled out in release builds.
	 *
	 * @param[in] message Error text
	 * @return Always false
	 */
	static bool ErrorDebug(const QString& message);
};
