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

#ifndef CC_SINUSX_FILTER_HEADER
#define CC_SINUSX_FILTER_HEADER

/**
 * @file SinusxFilter.h
 *
 * @brief Sinusx file filter
 *
 * I/O filter for Sinusx curve files.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief Sinusx file filter
 *
 * Read/write Sinusx curve files.
 */
class SinusxFilter : public FileIOFilter
{
  public:
	/// Constructor
	SinusxFilter();

	/// Load file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/// Check if can save
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;

	/// Save file
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;
};

#endif // CC_SINUSX_FILTER_HEADER
