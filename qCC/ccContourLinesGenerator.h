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
 * @brief Contour lines generator
 *
 * Generator for contour lines from raster grids.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
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
 * @brief Contour lines generator
 *
 * Generate contour lines from raster grids.
 */
class ccContourLinesGenerator
{
  public:
	/**
	 * @brief Generation parameters
	 */
	struct Parameters
	{
		double         startAltitude             = 0.0;
		double         maxAltitude               = 0.0;
		double         step                      = 0.0;
		ccScalarField* altitudes                 = nullptr;
		int            minVertexCount            = 3;
		bool           projectContourOnAltitudes = false;
		double         emptyCellsValue           = std::numeric_limits<double>::quiet_NaN();
		QWidget* parentWidget  = nullptr;
		bool     ignoreBorders = false;
	};

	/**
	 * @brief Generate contour lines
	 * @param[in] rasterGrid Raster grid
	 * @param[in] gridMinCornerXY Grid min corner
	 * @param[in] params Generation parameters
	 * @param[out] contourLines Generated contour lines
	 * @return true on success
	 */
	static bool GenerateContourLines(ccRasterGrid*             rasterGrid,
	                                 const CCVector2d&         gridMinCornerXY,
	                                 const Parameters&         params,
	                                 std::vector<ccPolyline*>& contourLines);
};
