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
 * @file ccKdTree.h
 *
 * @brief KD-tree structure for spatial point cloud operations.
 *
 * @details KD-tree for efficient spatial queries on point clouds.
 *
 * ## Overview
 *
 * A KD-tree (k-dimensional tree) organizes points in a spatial
 * hierarchy for fast nearest-neighbor queries:
 * - K-nearest neighbors search
 * - Points within radius search
 * - Box queries
 *
 * ## Usage
 *
 * @code
 * // Build KD-tree
 * ccKdTree* kdtree = new ccKdTree(cloud);
 * kdtree->build();
 *
 * // Find nearest neighbor
 * CCCoreLib::NearestNeighborIndexesAndDeviation nn;
 * kdtree->findNearestNeighbor(point, nn);
 *
 * // Find k nearest
 * kdtree->findNearestNeighbors(point, k, results);
 *
 * // Find within radius
 * std::vector<unsigned> indices;
 * kdtree->findPointsWithinRadius(point, radius, indices);
 * @endcode
 *
 * @extends CCCoreLib::TrueKdTree
 * @extends ccHObject
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see CCCoreLib::TrueKdTree for base implementation
 */

#ifndef CC_KD_TREE_HEADER
#define CC_KD_TREE_HEADER

// CCCoreLib
#include <TrueKdTree.h>

// Local
#include "ccHObject.h"

// System
#include <unordered_set>

class ccGenericPointCloud;

/**
 * @brief KD-tree structure.
 *
 * @details Extends CCCoreLib::TrueKdTree with ccHObject interface
 * for integration with CloudCompare's hierarchy.
 *
 * @extends CCCoreLib::TrueKdTree
 * @extends ccHObject
 */
class QCC_DB_LIB_API ccKdTree : public CCCoreLib::TrueKdTree
    , public ccHObject
{
  public:
	/**
	 * @brief Default constructor.
	 *
	 * @param[in] aCloud Point cloud to index.
	 */
	explicit ccKdTree(ccGenericPointCloud* aCloud);

	/**
	 * @brief Multiply bounding box.
	 *
	 * @param[in] multFactor Scale factor.
	 *
	 * @note Use when cloud is scaled without rebuilding tree.
	 */
	void multiplyBoundingBox(const PointCoordinateType multFactor);

	/**
	 * @brief Translate bounding box.
	 *
	 * @param[in] T Translation vector.
	 *
	 * @note Use when cloud is translated without rebuilding tree.
	 */
	void translateBoundingBox(const CCVector3& T);

	/**
	 * @brief Get class type.
	 */
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::POINT_KDTREE;
	}

	/**
	 * @brief Get own bounding box.
	 */
	virtual ccBBox getOwnBB(bool withGLFeatures = false) override;

	/**
	 * @brief Flag points with cell index.
	 *
	 * @param[out] sf Scalar field to store indices.
	 */
	void flagPointsWithCellIndex(CCCoreLib::ScalarField* sf);
};

#endif // CC_KD_TREE_HEADER
