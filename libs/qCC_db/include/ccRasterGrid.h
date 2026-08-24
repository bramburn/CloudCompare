// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccRasterGrid.h
 *
 * @brief Raster grid representation for point clouds.
 *
 * @details Provides 2.5D raster grid structure for discretizing
 * point clouds into a regular grid for elevation models, DSM, DTM,
 * and other raster-based processing.
 *
 * ## Overview
 *
 * A raster grid discretizes 3D space into cells:
 * - Each cell stores height statistics (min, max, mean)
 * - Color can be aggregated per cell
 * - Point indices can be linked for detailed analysis
 *
 * ## Grid Cell
 *
 * Each ccRasterCell stores:
 * - **Height**: Grid elevation value
 * - **Min/Max height**: Height range in cell
 * - **Point count**: Points projected to cell
 * - **Nearest point**: Closest point to cell center
 * - **Color**: Aggregated color
 * - **Point refs**: Linked list of point references
 *
 * ## Interpolation
 *
 * Supports Kriging interpolation for filling gaps:
 * - Ordinary Kriging
 * - Simple Kriging
 * - Automatic variogram fitting
 *
 * ## Usage
 *
 * @code
 * // Compute grid size
 * unsigned width, height;
 * ccRasterGrid::ComputeGridSize(Z, bbox, gridStep, width, height);
 *
 * // Create raster
 * ccRasterGrid raster;
 * raster.init(width, height);
 *
 * // Fill from point cloud
 * FillWithPoints(cloud, raster, gridStep, bbox);
 *
 * // Interpolate gaps
 * OrdinaryKriging(raster, variogram);
 *
 * // Export to mesh
 * ccMesh* mesh = ccRasterGridToMesh(raster, gridStep);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPointCloud for point cloud source
 * @see ccMesh for mesh export
 */

#pragma once

// Local
#include "ccBBox.h"
#include "qCC_db.h"

// CCCoreLib
#include <Kriging.h>

// System
#include <limits>

class ccGenericPointCloud;
class ccPointCloud;
class ccProgressDialog;

/**
 * @brief Raster grid cell.
 *
 * @details Stores statistics for one grid cell.
 */
struct QCC_DB_LIB_API ccRasterCell
{
	/**
	 * @brief Default constructor.
	 */
	ccRasterCell()
	    : h(std::numeric_limits<double>::quiet_NaN())
	    , minHeight(0)
	    , maxHeight(0)
	    , nbPoints(0)
	    , nearestPointIndex(0)
	    , color(0, 0, 0)
	    , pointRefHead(nullptr)
	    , pointRefTail(nullptr)
	{
	}

	/**
	 * @brief Get all point indexes projected to this cell.
	 *
	 * @param[out] indexes Point indexes.
	 * @param[in] pointRefList Point reference list.
	 */
	void getPointIndexes(std::vector<unsigned>& indexes, const std::vector<void*>& pointRefList) const;

	//! Height value (grid elevation).
	double h;

	//! Min height value in cell.
	PointCoordinateType minHeight;

	//! Max height value in cell.
	PointCoordinateType maxHeight;

	//! Number of points projected to cell.
	unsigned nbPoints;

	//! Nearest point index to cell center.
	unsigned nearestPointIndex;

	//! Aggregated color (RGB).
	CCVector3d color;

	//! First point reference in linked list.
	void** pointRefHead;

	//! Last point reference in linked list.
	void** pointRefTail;
};

/**
 * @brief Raster grid structure.
 *
 * @details 2.5D raster representation for point clouds.
 */
struct QCC_DB_LIB_API ccRasterGrid
{
	/**
	 * @brief Default constructor.
	 */
	ccRasterGrid();

	/**
	 * @brief Destructor.
	 */
	virtual ~ccRasterGrid();

	/**
	 * @brief Compute grid dimensions.
	 *
	 * @param[in] Z Projection dimension (0=X, 1=Y, 2=Z).
	 * @param[in] box Bounding box.
	 * @param[in] gridStep Cell size.
	 * @param[out] width Grid width.
	 * @param[out] height Grid height.
	 *
	 * @return true if successful.
	 */
	static bool ComputeGridSize(unsigned char Z,
	                            const ccBBox& box,
	                            double gridStep,
	                            unsigned& width,
	                            unsigned& height);

	/**
	 * @brief Initialize empty grid.
	 *
	 * @param[in] width Grid width.
	 * @param[in] height Grid height.
	 *
	 * @return true if successful.
	 */
	bool init(unsigned width, unsigned height);

	/**
	 * @brief Clear grid.
	 */
	void clear();

	/**
	 * @brief Fill with points from cloud.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] Z Projection dimension.
	 * @param[in] gridStep Cell size.
	 * @param[in] bbox Bounding box.
	 * @param[in] progress Progress callback.
	 *
	 * @return true if successful.
	 */
	bool fillWithPoints(ccGenericPointCloud& cloud,
	                    unsigned Z,
	                    double gridStep,
	                    const ccBBox& bbox,
	                    ccProgressDialog* progress = nullptr);

	//! Grid width.
	unsigned width;

	//! Grid height.
	unsigned height;

	//! Grid cells.
	std::vector<ccRasterCell> cells;

	//! Bounding box.
	ccBBox bbox;

	//! Grid step (cell size).
	double gridStep;

	//! Projection dimension.
	unsigned char Z;
};

// ... (continues in implementation)

#endif // CC_RASTER_GRID_HEADER
