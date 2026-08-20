/**
 * @file LasTiler.h
 *
 * @brief LAS tiling utilities.
 *
 * @details Functions for tiling LAS/LAZ files into grid-based tiles.
 *
 * ## Tiling Dimensions
 *
 * - XY: Tile in X-Y plane
 * - XZ: Tile in X-Z plane
 * - YZ: Tile in Y-Z plane
 *
 * @author Thomas Montaigu
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        //
// ##########################################################################

#include <FileIOFilter.h>
#include <QString>
#include <laszip/laszip_api.h>

/**
 * @brief Tiling dimension enumeration.
 */
enum class LasTilingDimensions
{
	XY = 0, //!< Tile in X-Y plane.
	XZ = 1, //!< Tile in X-Z plane.
	YZ = 2, //!< Tile in Y-Z plane.
};

/**
 * @brief LAS tiling options.
 */
struct LasTilingOptions final
{
	//! Output directory.
	QString outputDir;

	//! Tiling dimensions.
	LasTilingDimensions dims = LasTilingDimensions::XY;

	//! Number of tiles in dimension 0.
	unsigned numTiles0 = 0;

	//! Number of tiles in dimension 1.
	unsigned numTiles1 = 0;

	/**
	 * @brief Get index 0.
	 *
	 * @return Index based on dimensions.
	 */
	inline size_t index0() const
	{
		switch (dims)
		{
		case LasTilingDimensions::XY:
			Q_FALLTHROUGH();
		case LasTilingDimensions::XZ:
			return 0;
		case LasTilingDimensions::YZ:
			return 1;
		}
		return 0;
	}

	/**
	 * @brief Get index 1.
	 *
	 * @return Index based on dimensions.
	 */
	inline size_t index1() const
	{
		switch (dims)
		{
		case LasTilingDimensions::XY:
			return 1;
		case LasTilingDimensions::XZ:
			Q_FALLTHROUGH();
		case LasTilingDimensions::YZ:
			return 2;
		}
		return 1;
	}
};

/**
 * @brief Tile a LAS reader into a grid.
 *
 * Takes ownership of the reader and handles closing/deleting.
 *
 * @param[in] laszipReader LASzip reader.
 * @param[in] originName Original file name.
 * @param[in] options Tiling options.
 *
 * @return Error code.
 */
CC_FILE_ERROR TileLasReader(laszip_POINTER laszipReader, const QString& originName, const LasTilingOptions& options);
