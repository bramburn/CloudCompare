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
// #                         COPYRIGHT: CNRS / OSUR                         #
// #                                                                        #
// ##########################################################################

#ifndef CC_LAS_FWF_FILTER_HEADER
#define CC_LAS_FWF_FILTER_HEADER

/**
 * @file LASFWFFilter.h
 *
 * @brief LAS FWF file filter
 *
 * I/O filter for LAS Full WaveForm files.
 *
 * @author CNRS / OSUR
 */

// qCC_io
#include <FileIOFilter.h>

/**
 * @brief LAS FWF file filter
 *
 * Read/write LAS files with Full WaveForm support.
 */
class LASFWFFilter : public FileIOFilter
{
  public:
	/// Constructor
	LASFWFFilter();

	/// Get file filter string
	static inline QString GetFileFilter()
	{
		return "LAS 1.3 or 1.4 (*.las *.laz)";
	}

	/// Load file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/// Check if can save
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;
	
	/// Save file
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;
};

#endif // CC_LAS_FWF_FILTER_HEADER
