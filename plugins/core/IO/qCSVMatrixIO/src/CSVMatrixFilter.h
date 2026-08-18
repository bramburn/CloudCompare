// ##########################################################################
// #                                                                        #
// #                  CLOUDCOMPARE PLUGIN: qCSVMatrixIO                     #
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
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

#ifndef CC_CSV_MATRIX_FILTER_HEADER
#define CC_CSV_MATRIX_FILTER_HEADER

/**
 * @file CSVMatrixFilter.h
 *
 * @brief CSV Matrix file filter
 *
 * I/O filter for CSV matrix files (2.5D clouds).
 *
 * @author Daniel Girardeau-Montaut
 */

// qCC_io
#include <FileIOFilter.h>

/**
 * @brief CSV Matrix file filter
 *
 * Read CSV matrix files.
 */
class CSVMatrixFilter : public FileIOFilter
{
  public:
	/// Constructor
	CSVMatrixFilter();

	/// Load file
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;
};

#endif // CC_CSV_MATRIX_FILTER_HEADER
