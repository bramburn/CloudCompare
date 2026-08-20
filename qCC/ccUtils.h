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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

/**
 * @file ccUtils.h
 *
 * @brief Utility functions
 *
 * Common utility functions for CloudCompare.
 *
 * @author CloudCompare project
 */

class QString;

// CCCoreLib
#include <CCGeom.h>

/**
 * @brief Utility functions namespace
 */
namespace ccUtils
{
	/**
	 * @brief Display locked vertices warning
	 * @param[in] meshName Mesh name
	 * @param[in] displayAsError Display as error
	 */
	void DisplayLockedVerticesWarning(const QString& meshName, bool displayAsError);

	/**
	 * @brief Get vector from clipboard
	 * @param[out] vector Vector result
	 * @param[in] sendErrors Send errors to console
	 * @return true if successful
	 */
	bool GetVectorFromClipboard(CCVector3d& vector, bool sendErrors = true);
} // namespace ccUtils
