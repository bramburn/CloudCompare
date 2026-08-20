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
 * @file ccContourLinesGenerator.h
 *
 * @brief Contour lines generator from raster grids.
 *
 * @details Generates contour lines (isolines) from raster grids using
 * the marching squares algorithm. Contour lines connect points
 * of equal value (typically elevation or height).
 *
 * The generator:
 * - Extracts contour lines at specified altitude levels
 * - Filters contours by minimum vertex count
 * - Optionally projects contours onto altitude surfaces
 * - Handles empty/no-data cells
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccIsolines
 * @see ccRasterGrid
 */

struct ccRasterGrid;
class ccPolyline;
class ccScalarField;
class QWidget;

// CCCoreLib
#include <CCGeom.h>

// system
#include <vector>

/**
 * @brief Generator for contour lines from raster grids.
 *
 * @details Provides static methods for generating contour lines
 * (isolines) from raster grids. Contour lines are polylines
 * connecting points of equal value in the raster.
 *
 * Usage example:
 * @code
 * ccContourLinesGenerator::Parameters params;
 * params.startAltitude = 0.0;
 * params.maxAltitude = 100.0;
 * params.step = 10.0;  // 10m contours
 * params.minVertexCount = 3;  // Ignore small contours
 *
 * std::vector<ccPolyline*> contours;
 * ccContourLinesGenerator::GenerateContourLines(
 *     raster, gridOrigin, params, contours);
 * @endcode
 */
class ccContourLinesGenerator
{
  public:
	/**
	 * @brief Parameters for contour line generation.
	 */
	struct Parameters
	{
		//! Starting altitude for contour generation
		double startAltitude = 0.0;

		//! Maximum altitude for contour generation
		double maxAltitude = 0.0;

		//! Altitude step between contours
		double step = 0.0;

		//! Optional scalar field containing altitude values per cell
		/** If set, overrides startAltitude/maxAltitude/step.
		 *  Each cell can have a different base altitude.
		 */
		ccScalarField* altitudes = nullptr;

		//! Minimum vertex count for output contours
		/** Contours with fewer vertices than this are discarded.
		 *  Set to 0 to disable filtering.
		 */
		int minVertexCount = 3;

		//! Whether to project contours onto altitude surfaces
		/** If true, Z values are interpolated from the raster.
		 */
		bool projectContourOnAltitudes = false;

		//! Value representing empty/no-data cells
		double emptyCellsValue = std::numeric_limits<double>::quiet_NaN();

		//! Parent widget for progress dialog
		QWidget* parentWidget = nullptr;

		//! Whether to ignore raster border cells
		bool ignoreBorders = false;
	};

	/**
	 * @brief Generate contour lines from a raster grid.
	 *
	 * @param[in] rasterGrid Raster grid to extract contours from.
	 * @param[in] gridMinCornerXY Minimum corner of the grid in world coords.
	 * @param[in] params Generation parameters.
	 * @param[out] contourLines Generated contour polylines.
	 *
	 * @return true on success, false on error.
	 *
	 * @details Extracts contour lines from the raster at the specified
	 * altitude intervals. The number of contours generated depends
	 * on the range (maxAltitude - startAltitude) / step.
	 *
	 * Each generated contour is a ccPolyline that can be:
	 * - Added to the database tree
	 * - Exported to file
	 * - Styled (color, width, etc.)
	 *
	 * @note The caller owns the returned polylines and is
	 * responsible for deleting them.
	 */
	static bool GenerateContourLines(ccRasterGrid*             rasterGrid,
	                                 const CCVector2d&         gridMinCornerXY,
	                                 const Parameters&         params,
	                                 std::vector<ccPolyline*>& contourLines);
};
