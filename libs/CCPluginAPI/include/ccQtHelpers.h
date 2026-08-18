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
 * @file ccQtHelpers.h
 *
 * @brief Qt helper utilities
 *
 * Utility functions for Qt widgets and threading.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Qt
#include <QAbstractButton>
#include <QThread>

/**
 * @brief Qt helper functions
 */
class ccQtHelpers
{
  public:
	/**
	 * @brief Set button background color
	 * @param[in] button Button to modify
	 * @param[in] col Color to set
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
	 * @brief Get ideal thread count
	 * @param[in] idealThreadCount Base thread count
	 * @return Adjusted thread count
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
	 * @brief Get ideal thread count (uses Qt)
	 * @return Ideal thread count for this machine
	 */
	static int GetMaxThreadCount()
	{
		return GetMaxThreadCount(QThread::idealThreadCount());
	}
};
