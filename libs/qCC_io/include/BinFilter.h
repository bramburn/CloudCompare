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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

/**
 * @file BinFilter.h
 *
 * @brief Binary file filter
 *
 * CloudCompare native binary format filter.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief Binary filter
 *
 * CloudCompare native .bin format filter.
 */
class QCC_IO_LIB_API BinFilter : public FileIOFilter
{
  public:
	/// Constructor
	BinFilter();

	/// Get file filter string
	static inline QString GetFileFilter()
	{
		return "CloudCompare entities (*.bin)";
	}

	/// Get default extension
	static inline QString GetDefaultExtension()
	{
		return "bin";
	}

	/// Get last saved file version
	static short GetLastSavedFileVersion();

	// inherited from FileIOFilter
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;

	/// Load old format
	static CC_FILE_ERROR LoadFileV1(QFile& in, ccHObject& container, unsigned nbScansTotal, const LoadParameters& parameters);

	/// Load new format
	static CC_FILE_ERROR LoadFileV2(QFile& in, ccHObject& container, int flags, bool parallel, QWidget* parentWidget = nullptr);

	/// Save new format
	static CC_FILE_ERROR SaveFileV2(QFile& out, ccHObject* object);
};
