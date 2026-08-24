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
// #                        COPYRIGHT: CNRS / OSUR                          #
// #                                                                        #
// ##########################################################################

#ifndef CC_SIMPLE_BIN_FILTER_HEADER
#define CC_SIMPLE_BIN_FILTER_HEADER

/**
 * @file SimpleBinFilter.h
 *
 * @brief Simple binary file filter
 *
 * Simple binary file I/O with attached text meta-file.
 *
 * @author CNRS / OSUR
 */

#include "FileIOFilter.h"

/**
 * @brief Simple binary file filter
 *
 * Read/write simple binary files with meta-data.
 */
class SimpleBinFilter : public FileIOFilter
{
  public:
	/// Constructor
	SimpleBinFilter();

	/// Load file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/// Check if can save
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;

	/// Save file
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;
};

#endif // CC_SIMPLE_BIN_FILTER_HEADER
