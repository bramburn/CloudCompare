/**
 * @file LasTiler.h
 *
 * @brief LAS/LAZ file tiling utilities
 *
 * Functions for splitting a LAS/LAZ file into a grid of tiles, each
 * saved as a separate output file. Used for processing very large point
 * cloud files that don't fit in memory.
 *
 * ## Tiling Strategy
 *
 * The file is split along two of the three coordinate axes (chosen via
 * LasTilingDimensions) into numTiles0 × numTiles1 tiles. Each tile
 * covers a contiguous region of the point cloud's bounding box.
 *
 * ## Output
 *
 * Each tile is written as a separate LAS/LAZ file in the output directory,
 * named by appending the tile index to the original filename:
 * `original_tile_0_0.las`, `original_tile_0_1.las`, etc.
 *
 * No full point cloud is loaded into memory — points are streamed from
 * the laszip reader and written directly to the appropriate tile file.
 *
 * ## Dimension Mapping
 *
 * | Dimension | index0() | index1() |
 * |-----------|----------|----------|
 * | XY        | 0 (X)   | 1 (Y)   |
 * | XZ        | 0 (X)   | 2 (Z)   |
 * | YZ        | 1 (Y)   | 2 (Z)   |
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
 * @brief Tile a LAS/LAZ reader into a grid of output files
 *
 * Takes ownership of the laszip reader and handles closing/deleting.
 * Points are streamed from the reader and written directly to the
 * appropriate output tile file based on their coordinates.
 *
 * @param[in] laszipReader Open laszip reader (takes ownership)
 * @param[in] originName Original file name (used as base for output names)
 * @param[in] options Tiling options (grid size, dimensions, output dir)
 * @return CC_FILE_ERROR status
 */
CC_FILE_ERROR TileLasReader(laszip_POINTER laszipReader, const QString& originName, const LasTilingOptions& options);
