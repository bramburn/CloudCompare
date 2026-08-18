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

#ifndef CC_SOI_FILTER_HEADER
#define CC_SOI_FILTER_HEADER

/**
 * @file SoiFilter.h
 *
 * @brief Soisic file filter
 *
 * I/O filter for Mensi Soisic point cloud files.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief Soisic file filter
 *
 * Read Mensi Soisic point cloud files.
 */
class SoiFilter : public FileIOFilter
{
  public:
	/// Constructor
	SoiFilter();

	/// Load Soisic file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;
};

#endif // CC_SOI_FILTER_HEADER
