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
// #                       COPYRIGHT: SAGE INGENIERIE                       #
// #                                                                        #
// ##########################################################################

/**
 * @file HeightProfileFilter.h
 *
 * @brief Height profile filter
 *
 * Polyline height profile I/O filter.
 */

#ifndef CC_HEIGHT_PROFILE_HEADER
#define CC_HEIGHT_PROFILE_HEADER

#include "FileIOFilter.h"

/**
 * @class HeightProfileFilter
 *
 * @brief Height profile filter
 *
 * Polyline height profile I/O filter.
 */
class HeightProfileFilter : public FileIOFilter
{
  public:
	HeightProfileFilter();

	// inherited from FileIOFilter
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;
};

#endif // CC_HEIGHT_PROFILE_HEADER
