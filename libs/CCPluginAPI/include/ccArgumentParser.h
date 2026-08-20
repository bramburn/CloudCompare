// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        //
// ##########################################################################

/**
 * @file ccArgumentParser.h
 *
 * @brief Command line argument parser for CloudCompare CLI.
 *
 * @details Provides a type-safe argument parser for processing command-line
 * arguments in batch processing mode.
 *
 * ## Features
 *
 * - Type-safe parsing (int, float, double, unsigned)
 * - Enum parsing with case-insensitive matching
 * - Option recognition (e.g., `-o`, `-SILENT`)
 * - Range validation
 * - Automatic error logging
 *
 * ## Usage
 *
 * @code
 * // Parse arguments for a command
 * ccCommandLineInterface cmd;
 * cmd.addCommand("CROP", "Crop a cloud",
 *     [](ccCommandLineInterface& cmd) {
 *         auto parser = cmd.getArgumentParser();
 *
 *         // Parse options
 *         bool silent = parser->tryConsumeOption("SILENT");
 *
 *         // Parse required arguments
 *         auto minCorner = parser->takeDouble("min corner");
 *         auto maxCorner = parser->takeDouble("max corner");
 *
 *         if (!minCorner || !maxCorner) {
 *             return false;
 *         }
 *
 *         // Process...
 *         return true;
 *     });
 * @endcode
 *
 * @author CloudCompare project
 *
 * @see ccCommandLineInterface for command registration
 */

#pragma once

#include "CCPluginAPI.h"

#include <ccLog.h>

// Qt
#include <QObject>
#include <QStringList>

// System
#include <initializer_list>
#include <optional>
#include <utility>

/**
 * @brief Command line argument parser.
 *
 * @details Parses and validates command-line arguments with
 * automatic error logging.
 *
 * Features:
 * - Type-safe argument extraction
 * - Option recognition
 * - Range validation
 * - Enum parsing
 */
class CCPLUGIN_LIB_API ccArgumentParser
{
  public:
	/**
	 * @brief Construct parser with arguments.
	 *
	 * @param[in] arguments Argument list (modified in place).
	 */
	explicit ccArgumentParser(QStringList& arguments);

	/**
	 * @brief Peek at next argument without consuming.
	 *
	 * @return Next argument, or nullptr if empty.
	 */
	const QString peek() const;

	/**
	 * @brief Skip next argument.
	 *
	 * Use after peek().
	 */
	void skip();

	/**
	 * @brief Check if arguments are empty.
	 *
	 * @return true if no arguments left.
	 */
	bool isEmpty() const;

	/**
	 * @brief Get number of remaining arguments.
	 *
	 * @return Argument count.
	 */
	size_t size() const
	{
		return m_arguments.size();
	}

	/**
	 * @brief Take and return next argument.
	 *
	 * @return Next argument, or null string.
	 */
	QString takeNext();

	/**
	 * @brief Take and parse next argument as float.
	 *
	 * @param[in] context Parameter name for error messages.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	std::optional<float> takeFloat(const QString& context);

	/**
	 * @brief Take and parse next argument as double.
	 *
	 * @param[in] context Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	std::optional<double> takeDouble(const QString& context, double min = std::numeric_limits<double>::lowest(), double max = std::numeric_limits<double>::max());

	/**
	 * @brief Take and parse next argument as int.
	 *
	 * @param[in] context Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	std::optional<int> takeInt(const QString& context, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());

	/**
	 * @brief Take and parse next argument as unsigned.
	 *
	 * @param[in] context Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	std::optional<unsigned> takeUInt(const QString& context, unsigned min = 0, unsigned max = std::numeric_limits<unsigned>::max());

	/**
	 * @brief Try to consume an option flag.
	 *
	 * @param[in] option Option name (without `-`).
	 *
	 * @return true if option was found and consumed.
	 *
	 * @note Case insensitive.
	 */
	bool tryConsumeOption(const QString& option);

	/**
	 * @brief Take and parse next argument as enum.
	 *
	 * @tparam T Enum type.
	 * @param[in] mapping String-to-enum mapping.
	 * @param[in] context Parameter name.
	 *
	 * @return Parsed enum, or std::nullopt.
	 *
	 * @note Strings should be UPPER_CASE; input is case insensitive.
	 */
	template <typename T>
	std::optional<T> takeEnum(const std::initializer_list<std::pair<const char*, T>>& mapping, const QString& context)
	{
		if (m_arguments.isEmpty())
		{
			ccLog::Error(QObject::tr("Missing parameter: %1").arg(context));
			return std::nullopt;
		}

		const QString arg = m_arguments.takeFirst();
		return ParseEnum(arg, mapping, context);
	}

	/**
	 * @brief Parse string as enum.
	 *
	 * @tparam T Enum type.
	 * @param[in] arg String to parse.
	 * @param[in] mapping String-to-enum mapping.
	 * @param[in] context Parameter name.
	 *
	 * @return Parsed enum, or std::nullopt.
	 */
	template <typename T>
	static std::optional<T> ParseEnum(const QString& arg, const std::initializer_list<std::pair<const char*, T>>& mapping, const QString& context)
	{
		const QString upper = arg.toUpper();
		for (const auto& [key, value] : mapping)
		{
			if (key == upper)
			{
				return value;
			}
		}

		QStringList valid;
		for (const auto& [key, value] : mapping)
		{
			valid << key;
		}

		ccLog::Error(QObject::tr("Invalid %1: '%2' (expected one of: %3)")
		                 .arg(context, arg, valid.join(", ")));

		return std::nullopt;
	}

	/**
	 * @brief Parse string as float.
	 *
	 * @param[in] arg String to parse.
	 * @param[in] name Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	static std::optional<float> ParseFloat(const QString& arg, const QString& name, float min = std::numeric_limits<float>::lowest(), float max = std::numeric_limits<float>::max());

	/**
	 * @brief Parse string as double.
	 *
	 * @param[in] arg String to parse.
	 * @param[in] name Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	static std::optional<double> ParseDouble(const QString& arg, const QString& name, double min = std::numeric_limits<double>::lowest(), double max = std::numeric_limits<double>::max());

	/**
	 * @brief Parse string as int.
	 *
	 * @param[in] arg String to parse.
	 * @param[in] name Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	static std::optional<int> ParseInt(const QString& arg, const QString& name, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());

	/**
	 * @brief Parse string as unsigned.
	 *
	 * @param[in] arg String to parse.
	 * @param[in] name Parameter name.
	 * @param[in] min Minimum value.
	 * @param[in] max Maximum value.
	 *
	 * @return Parsed value, or std::nullopt.
	 */
	static std::optional<unsigned> ParseUInt(const QString& arg, const QString& name, unsigned min = 0, unsigned max = std::numeric_limits<unsigned>::max());

  private:
	//! Command line arguments.
	QStringList& m_arguments;
};
