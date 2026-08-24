#pragma once

// ##########################################################################
// #                                                                        #
// #                 CLOUDCOMPARE PLUGIN: qSTEPCADImport                    #
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
// #                          COPYRIGHT: EDF R&D                            #
// #                                                                        #
// ##########################################################################

/**
 * @file STEPFilter.h
 *
 * @brief STEP file filter
 *
 * I/O filter for STEP CAD files.
 *
 * @author EDF R&D
 */

#include <FileIOFilter.h>

/**
 * @class STEPFilter
 *
 * @brief STEP file filter
 *
 * Import STEP CAD files.
 */
class STEPFilter : public FileIOFilter
{
  public:
	/// Constructor
	STEPFilter();

	/// Load file
	CC_FILE_ERROR loadFile(const QString& fullFilename, ccHObject& container, LoadParameters& parameters) override;

	/**
	 * @brief Import STEP file
	 * @param[out] container Container for loaded entities
	 * @param[in] fullFilename Full path to file
	 * @param[in] linearDeflection Triangle deflection value
	 * @param[in] parameters Load parameters
	 * @return Error code
	 */
	CC_FILE_ERROR importStepFile(ccHObject& container,
	                             const QString& fullFilename,
	                             double linearDeflection,
	                             LoadParameters& parameters);

	/**
	 * @brief Set default linear deflection
	 * @param[in] value Linear deflection (in [1e-2, 1e-6])
	 *
	 * Smaller value = smaller triangles.
	 * If too large, tessellation may crash.
	 */
	static void SetDefaultLinearDeflection(double value);
};
