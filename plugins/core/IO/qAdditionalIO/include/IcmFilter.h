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

#ifndef CC_ICM_FILTER_HEADER
#define CC_ICM_FILTER_HEADER

/**
 * @file IcmFilter.h
 *
 * @brief ICM file filter
 *
 * I/O filter for calibrated images and cloud meta-files.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief ICM file filter
 *
 * Read calibrated images and cloud meta-files.
 */
class IcmFilter : public FileIOFilter
{
  public:
	/// Constructor
	IcmFilter();

	/// Load ICM file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

  private:
	/// Load calibrated images
	static int LoadCalibratedImages(ccHObject* entities, const QString& path, const QString& imageDescFilename, const ccBBox& globalBBox);
};

#endif // CC_ICM_FILTER_HEADER
