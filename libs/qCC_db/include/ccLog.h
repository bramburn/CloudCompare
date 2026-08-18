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
 * Provides a thread-safe logging interface used throughout CloudCompare
 * for console output, warnings, and error messages.
 *
 * @section Usage
 * @code
 * ccLog::Print("Loading file: %s", filename);
 * ccLog::Warning("Low memory warning");
 * ccLog::Error("Failed to open file: %s", filename);
 * @endcode
 *
 * @section Thread Safety
 * All logging methods are thread-safe.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "qCC_db.h"

// system
#include <stdio.h>
#include <string>

// Qt
#include <QString>

/**
 * @brief Main log interface
 *
 * Thread-safe logging interface meant to be used as a singleton.
 * All methods must be thread-safe as logging can occur from
 * multiple threads during processing operations.
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
	 * @brief Get the singleton instance
	 * @return Pointer to the current logging instance
	 */
	static ccLog* TheInstance();

	/**
	 * @brief Register a logging instance
	 * @param[in] logInstance Instance to register
	 */
	static void RegisterInstance(ccLog* logInstance);

	/**
	 * @brief Enable message backup system
	 *
	 * When enabled, messages are stored until a logging instance
	 * is registered. Useful during early initialization.
	 *
	 * @param[in] state true to enable backup, false to disable
	 */
	static void EnableMessageBackup(bool state);

	/**
	 * @brief Message severity levels
	 */
	enum MessageLevelFlags
	{
		LOG_VERBOSE   = 0, //!< Verbose/debug message
		LOG_STANDARD  = 1, //!< Standard informational message
		LOG_IMPORTANT = 2, //!< Important message (highlighted)
		LOG_WARNING   = 3, //!< Warning message
		LOG_ERROR     = 4, //!< Error message

		DEBUG_FLAG = 8 //!< Debug flag (reserved)
	};

	/**
	 * @brief Get the current verbosity level
	 * @return Current verbosity level
	 */
	static int VerbosityLevel();

	/**
	 * @brief Set the verbosity level
	 * @param[in] level New verbosity level
	 */
	static void SetVerbosityLevel(int level);

	/**
	 * @brief Log a message directly
	 * @param[in] message Message to log
	 * @param[in] level Message severity level
	 */
	static void LogMessage(const QString& message, int level);

	/**
	 * @brief Core logging method
	 *
	 * Pure virtual method that must be implemented by concrete classes.
	 *
	 * @param[in] message The message to log
	 * @param[in] level Message severity (see MessageLevelFlags)
	 *
	 * @warning MUST BE THREAD SAFE!
	 */
	virtual void logMessage(const QString& message, int level) = 0;

	/**
	 * @brief Print verbose message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool PrintVerbose(const char* format, ...);

	/**
	 * @brief Print verbose message (QString version)
	 * @param[in] message Message to print
	 * @return Always true
	 */
	static bool PrintVerbose(const QString& message);

	/**
	 * @brief Print standard message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool Print(const char* format, ...);

	/**
	 * @brief Print standard message (QString version)
	 * @param[in] message Message to print
	 * @return Always true
	 */
	static bool Print(const QString& message);

	/**
	 * @brief Print important message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool PrintHigh(const char* format, ...);

	/**
	 * @brief Print important message (QString version)
	 * @param[in] message Message to print
	 * @return Always true
	 */
	static bool PrintHigh(const QString& message);

	/**
	 * @brief Print debug message (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always true
	 */
	static bool PrintDebug(const char* format, ...);

	/**
	 * @brief Print debug message (QString version)
	 * @param[in] message Message to print
	 * @return Always true
	 */
	static bool PrintDebug(const QString& message);

	/**
	 * @brief Print warning (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining)
	 */
	static bool Warning(const char* format, ...);

	/**
	 * @brief Print warning (QString version)
	 * @param[in] message Warning message
	 * @return Always false (for chaining)
	 */
	static bool Warning(const QString& message);

	/**
	 * @brief Print debug warning (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining)
	 */
	static bool WarningDebug(const char* format, ...);

	/**
	 * @brief Print debug warning (QString version)
	 * @param[in] message Warning message
	 * @return Always false (for chaining)
	 */
	static bool WarningDebug(const QString& message);

	/**
	 * @brief Print error (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining)
	 */
	static bool Error(const char* format, ...);

	/**
	 * @brief Print error (QString version)
	 * @param[in] message Error message
	 * @return Always false (for chaining)
	 */
	static bool Error(const QString& message);

	/**
	 * @brief Print debug error (printf-style)
	 * @param[in] format Printf-style format string
	 * @return Always false (for chaining)
	 */
	static bool ErrorDebug(const char* format, ...);

	/**
	 * @brief Print debug error (QString version)
	 * @param[in] message Error message
	 * @return Always false (for chaining)
	 */
	static bool ErrorDebug(const QString& message);
};
