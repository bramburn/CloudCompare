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

#ifndef CC_CUSTOM_DOUBLE_VALIDATOR_HEADER
#define CC_CUSTOM_DOUBLE_VALIDATOR_HEADER

/**
 * @file ccCustomDoubleValidator.h
 *
 * @brief Custom double validator that accepts both comma and period as decimal separator.
 *
 * @details A QValidator subclass for validating double-precision floating-point
 * numbers entered by the user.
 *
 * Features:
 * - Accepts digits, minus sign, and decimal point
 * - Automatically converts European-style comma (,) to period (.)
 * - Allows locale-independent number input
 *
 * This is useful for European users who typically use comma as
 * the decimal separator in their locale.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see QValidator
 */

// Qt
#include <QString>
#include <QValidator>

/**
 * @brief Validator for double numbers.
 *
 * @details Validates double-precision number input with support
 * for both period (.) and comma (,) as decimal separators.
 *
 * The validator accepts:
 * - Digits (0-9)
 * - Minus sign (-) for negative numbers
 * - Period (.) as decimal separator
 * - Comma (,) which is automatically converted to period
 *
 * Example valid inputs:
 * - "123.45"
 * - "123,45" (comma converted to period)
 * - "-123.45"
 * - "0.001"
 *
 * @extends QValidator
 */
class ccCustomDoubleValidator : public QValidator
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the validator.
	 *
	 * @param[in] parent Parent QObject.
	 */
	explicit ccCustomDoubleValidator(QObject* parent = nullptr)
	    : QValidator(parent)
	{
	}

	/**
	 * @brief Validate input string.
	 *
	 * @param[in,out] input Input string to validate.
	 * @param[in,out] pos Cursor position (unused).
	 *
	 * @return Validation state:
	 *         - Acceptable: Input is valid
	 *         - Invalid: Input contains invalid characters
	 *
	 * @details Reimplemented from QValidator.
	 *
	 * Automatically replaces any commas with periods before validation.
	 * Only digits, minus signs, and periods are accepted.
	 */
	State validate(QString& input, int& pos) const override
	{
		for (int i = 0; i < input.size(); ++i)
		{
			QChar c = input[i];
			if (c == ',')
			{
				// European decimal separator - convert to period
				input[i] = '.';
				continue;
			}
			else if (c == '.' || c == '-' || c.isDigit())
			{
				// Valid: period, minus, or digit
				continue;
			}
			else
			{
				// Invalid character
				return Invalid;
			}
		}
		return Acceptable;
	}
};

#endif // CC_CUSTOM_DOUBLE_VALIDATOR_HEADER
