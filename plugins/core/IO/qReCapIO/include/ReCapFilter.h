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
// ##########################################################################

#pragma once

#include <FileIOFilter.h>

/**
 * @brief ReCap file filter
 *
 * Read Autodesk ReCap RCS (ReCap Station) and RCP (ReCap Photo) files
 * via the Autodesk ReCap SDK.
 *
 * Supported read attributes:
 *   - Cartesian / spherical positions
 *   - RGB colour  (RCAttributeType::Color)
 *   - Intensity  (RCAttributeType::Intensity)
 *   - Normals    (RCAttributeType::Normal)
 *   - Classification (RCAttributeType::Classification)
 *   - Segment IDs (RCAttributeType::SegmentId)
 *
 * Write is not implemented (ReCap SDK write path requires a full
 * project session and is export-oriented).
 */
class ReCapFilter : public FileIOFilter
{
  public:
	/// Constructor — registers supported file extensions and priority.
	ReCapFilter();

	/// Load an RCS or RCP file into \a container.
	/// Returns CC_FERR_NO_ERROR on success.
	CC_FILE_ERROR loadFile( const QString& filename,
	                        ccHObject& container,
	                        LoadParameters& parameters ) override;

	/// Read-only: ReCap files cannot be re-exported via this plugin.
	bool canSave( CC_CLASS_ENUM type,
	              bool& multiple,
	              bool& exclusive ) const override;

	/// Write is not supported.
	CC_FILE_ERROR saveToFile( ccHObject* entity,
	                          const QString& filename,
	                          const SaveParameters& parameters ) override;
};
