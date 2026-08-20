// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        //
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccQtHelpers.h
 *
 * @brief Qt utility helper functions.
 *
 * @details Provides common utility functions for Qt widgets
 * and threading operations.
 *
 * ## Overview
 *
 * This namespace/class provides:
 * - Button styling utilities
 * - Thread count management
 *
 * ## Usage
 *
 * @code
 * // Set button background color
 * ccQtHelpers::SetButtonColor(myButton, Qt::red);
 *
 * // Get optimal thread count for processing
 * int threads = ccQtHelpers::GetMaxThreadCount();
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#pragma once

// Qt
#include <QAbstractButton>
#include <QThread>

/**
 * @brief Qt helper utilities.
 *
 * @details Common utility functions for Qt operations.
 */
class ccQtHelpers
{
  public:
	/**
	 * @brief Set button background color.
	 *
	 * @param[in] button Button to modify.
	 * @param[in] col Color to set.
	 *
	 * @note Uses stylesheet for styling.
	 */
	static void SetButtonColor(QAbstractButton* button, const QColor& col)
	{
		if (button != nullptr)
		{
			button->setStyleSheet(QStringLiteral("* { background-color: rgb(%1,%2,%3) }")
			                          .arg(col.red())
			                          .arg(col.green())
			                          .arg(col.blue()));
		}
	}

	/**
	 * @brief Get optimal thread count.
	 *
	 * @param[in] idealThreadCount Base thread count.
	 *
	 * @return Adjusted thread count.
	 *
	 * @note Reduces count slightly for very high core counts
	 *       to avoid resource contention.
	 */
	static int GetMaxThreadCount(int idealThreadCount)
	{
		if (idealThreadCount <= 4)
		{
			return idealThreadCount;
		}
		else if (idealThreadCount <= 8)
		{
			return idealThreadCount - 1;
		}
		else
		{
			return idealThreadCount - 2;
		}
	}

	/**
	 * @brief Get optimal thread count.
	 *
	 * @return Ideal thread count for this machine.
	 *
	 * @note Uses QThread::idealThreadCount() as base.
	 */
	static int GetMaxThreadCount()
	{
		return GetMaxThreadCount(QThread::idealThreadCount());
	}
};
