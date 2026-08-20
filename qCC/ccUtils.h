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
 * @brief Common utility functions for CloudCompare.
 *
 * @details This file provides a namespace for miscellaneous utility functions
 * used throughout the CloudCompare application. Currently includes:
 * - Warning/error display for locked vertices
 * - Clipboard-based vector input
 *
 * @author CloudCompare project
 * @date 2024
 *
 * @see ccConsole
 */

class QString;

// CCCoreLib
#include <CCGeom.h>

/**
 * @brief Utility functions namespace.
 *
 * @details Contains miscellaneous helper functions that don't belong
 * to a specific subsystem. Functions handle common tasks like
 * displaying warnings and reading data from the clipboard.
 */
namespace ccUtils
{
	/**
	 * @brief Display a warning about locked vertices.
	 *
	 * @param[in] meshName The name of the mesh whose vertices are locked.
	 * @param[in] displayAsError If true, displays as an error message;
	 *                           if false, displays as a warning.
	 *
	 * @details In CloudCompare, mesh vertices can be shared between multiple
	 * entities (e.g., multiple meshes sharing the same point cloud). When
	 * operations that modify vertices are attempted, this warning informs
	 * the user that the vertices are locked and that they should operate
	 * directly on the vertices cloud.
	 *
	 * @note This is typically called when a mesh operation fails because
	 * the underlying point cloud is shared.
	 *
	 * @par Example:
	 * @code
	 * ccUtils::DisplayLockedVerticesWarning("MyMesh", true);
	 * @endcode
	 */
	void DisplayLockedVerticesWarning(const QString& meshName, bool displayAsError);

	/**
	 * @brief Read a 3D vector from the system clipboard.
	 *
	 * @param[out] vector The parsed 3D vector. Only modified if successful.
	 * @param[in] sendErrors If true, error messages are sent to the console.
	 *
	 * @return true if a valid 3D vector was successfully parsed from the
	 *         clipboard, false otherwise.
	 *
	 * @details Parses the clipboard text as a 3D vector. Accepts various
	 * formats:
	 * - Space-separated: "1.5 2.5 3.5"
	 * - Semicolon-separated: "1.5; 2.5; 3.5"
	 * - Comma-separated: "1.5, 2.5, 3.5"
	 * - Bracketed: "[1.5, 2.5, 3.5]" or "{1.5 2.5 3.5}" or "(1.5, 2.5, 3.5)"
	 *
	 * @note The vector values are stored as CCVector3d (double precision).
	 *
	 * @par Example:
	 * @code
	 * CCVector3d vec;
	 * if (ccUtils::GetVectorFromClipboard(vec, true)) {
	 *     // Use vec.x(), vec.y(), vec.z()
	 * }
	 * @endcode
	 */
	bool GetVectorFromClipboard(CCVector3d& vector, bool sendErrors = true);
} // namespace ccUtils
