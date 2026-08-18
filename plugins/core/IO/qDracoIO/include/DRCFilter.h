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

#ifndef CC_DRC_FILTER_HEADER
#define CC_DRC_FILTER_HEADER

/**
 * @file DRCFilter.h
 *
 * @brief Draco file filter
 *
 * I/O filter for Google Draco compressed files.
 *
 * @author CloudCompare project
 */

#include <FileIOFilter.h>

/**
 * @brief Draco file filter
 *
 * Read/write Google Draco compressed files.
 */
class DRCFilter : public FileIOFilter
{
  public:
	/// Constructor
	DRCFilter();

	/// Load file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/// Check if can save
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;
	
	/// Save file
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;
};

#endif // CC_DRC_FILTER_HEADER
