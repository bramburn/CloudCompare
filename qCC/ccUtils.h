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
 * Common utility functions for CloudCompare that don't belong to any
 * specific class. Currently covers clipboard operations and locked
 * vertex warnings.
 *
 * @author CloudCompare project
 */

class QString;

// CCCoreLib
#include <CCGeom.h>

/**
 * @brief Utility functions namespace
 *
 * Provides miscellaneous helper functions used throughout the application.
 * All functions are free functions rather than members of a class.
 */
namespace ccUtils
{
	/**
	 * @brief Display a warning about locked mesh vertices
	 *
	 * Issues a warning (or error) when an operation is attempted on a mesh
	 * whose vertices are locked because they are shared with other entities
	 * (e.g., multiple meshes sharing the same point cloud). This is a common
	 * user mistake when working with merged entities.
	 *
	 * @param meshName       Name of the mesh whose vertices are locked
	 * @param displayAsError If true, display as an error; otherwise as a warning
	 */
	void DisplayLockedVerticesWarning(const QString& meshName, bool displayAsError);

	/**
	 * @brief Read a 3D vector from the system clipboard
	 *
	 * Attempts to parse the system clipboard contents as a 3D vector.
	 * Accepts the following formats (with optional surrounding brackets):
	 * - Space-separated: "1.0 2.0 3.0"
	 * - Semicolon-separated: "1.0; 2.0; 3.0"
	 * - Comma-separated: "1.0, 2.0, 3.0"
	 * - With brackets: "[1.0, 2.0, 3.0]", "(1.0 2.0 3.0)", "{1.0; 2.0; 3.0}"
	 *
	 * @param[out] vector    Output vector (populated on success)
	 * @param[in] sendErrors If true, log parsing errors to the console
	 * @return true if a valid 3-component vector was read, false otherwise
	 */
	bool GetVectorFromClipboard(CCVector3d& vector, bool sendErrors = true);
} // namespace ccUtils
