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

#ifndef CC_PN_FILTER_HEADER
#define CC_PN_FILTER_HEADER

/**
 * @file PNFilter.h
 *
 * @brief Point+Normal file filter
 *
 * Binary point cloud file I/O with point and normal data.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief Point+Normal file filter
 *
 * Read/write binary point cloud files with normals.
 */
class PNFilter : public FileIOFilter
{
  public:
	/// Constructor
	PNFilter();

	/// Load file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/// Check if can save
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;
	
	/// Save file
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;
};

#endif // CC_PN_FILTER_HEADER
