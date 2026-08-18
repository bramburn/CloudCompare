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

#ifndef CC_PTX_FILTER_HEADER
#define CC_PTX_FILTER_HEADER

/**
 * @file PTXFilter.h
 *
 * @brief PTX file filter
 *
 * I/O filter for PTX point cloud files.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief PTX file filter
 *
 * Read PTX point cloud files.
 */
class PTXFilter : public FileIOFilter
{
  public:
	/// Constructor
	PTXFilter();

	/// Load PTX file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;
};

#endif // CC_PTX_FILTER_HEADER
