#pragma once

//##########################################################################
//#                                                                        #
//#                     CLOUDCOMPARE PLUGIN: qFacets                       #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                      COPYRIGHT: Thomas Dewez, BRGM                     #
//#                                                                        #
//##########################################################################

/**
 * @file kdTreeForFacetExtraction.h
 *
 * @brief KD-tree for facet extraction
 *
 * KD-tree utilities for facet extraction.
 */

//qCC_db
#include <ccKdTree.h>

/**
 * @class ccKdTreeForFacetExtraction
 *
 * @brief KD-tree for facet extraction
 *
 * KD-tree utilities for facet extraction.
 */
class ccKdTreeForFacetExtraction
{
public:
	/**
	 * @brief Fuse cells
	 *
	 * Creates a new scalar fields with the groups indexes.
	 *
	 * @param kdTree KD-tree
	 * @param maxError Max error after fusion
	 * @param errorMeasure Error measure type
	 * @param maxAngle_deg Maximum angle between two sets to allow fusion (degrees)
	 * @param overlapCoef Maximum relative distance between two sets
	 * @param closestFirst Use closest first strategy
	 * @param progressCb Progress callback
	 * @return Success
	 */
	static bool FuseCells(	ccKdTree* kdTree,
							double maxError,
							CCCoreLib::DistanceComputationTools::MEASURE_TYPE errorMeasure,
							double maxAngle_deg,
							PointCoordinateType overlapCoef = 1,
							bool closestFirst = true,
							CCCoreLib::GenericProgressCallback* progressCb = nullptr);

};
