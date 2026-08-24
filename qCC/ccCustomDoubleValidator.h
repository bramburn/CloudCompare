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
 * @brief Custom double validator
 *
 * Validator for double numbers (accepts comma as decimal separator).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QString>
#include <QValidator>

/**
 * @brief Custom double validator
 *
 * Accepts double numbers, auto-replaces comma with period.
 */
class ccCustomDoubleValidator : public QValidator
{
	Q_OBJECT

  public:
	/**
	 * @brief Create validator
	 * @param[in] parent Parent
	 */
	explicit ccCustomDoubleValidator(QObject* parent = 0)
	    : QValidator(parent)
	{
	}

	// reimplemented from QValidator
	State validate(QString& input, int& pos) const
	{
		for (int i = 0; i < input.size(); ++i)
		{
			QChar c = input[i];
			if (c == ',')
			{
				input[i] = '.';
				continue;
			}
			else if (c == '.' || c == '-' || c.isDigit())
			{
				continue;
			}
			else
			{
				return Invalid;
			}
		}
		return Acceptable;
	}
};

#endif // CC_CUSTOM_DOUBLE_VALIDATOR_HEADER
