/**
 * @file CopcLoader.h
 *
 * @brief COPC (Cloud Optimized Point Cloud) loader.
 *
 * @details Loads COPC files, a variant of LAZ optimized for
 * cloud-based access with hierarchical octree structure.
 *
 * ## COPC Format
 *
 * COPC extends LAS/LAZ with:
 * - Hierarchical octree organization
 * - Chunk-based point storage
 * - Efficient spatial queries
 *
 * ## Usage
 *
 * @code
 * CopcLoader loader(header, filename);
 * if (!loader.isValid()) {
 *     return;
 * }
 *
 * // Set spatial constraint
 * loader.setClippingBoxConstraint(extent);
 *
 * // Get chunks to load
 * std::vector<std::reference_wrapper<ChunkInterval>> chunks;
 * uint64_t estimatedCount;
 * loader.getChunkIntervalsSet(chunks, estimatedCount);
 * @endcode
 *
 * @author Hobu, Inc.
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                            COPCLoader                                  #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Hobu, Inc.                           	    #
// #                                                                        //
// ##########################################################################

#include "CopcVlrs.h"
#include "LasDetails.h"

// Qt
#include <QFile>

// Laszip
#include <laszip/laszip_api.h>

// System
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// CCCoreLib
#include <CCGeom.h>
#include <ccLog.h>

using namespace LasDetails;

/**
 * @brief COPC support namespace.
 */
namespace copc
{
	/**
	 * @brief COPC file loader.
	 *
	 * Loads Cloud Optimized Point Cloud (COPC) files.
	 */
	class CopcLoader
	{
	  public: // methods
		/**
		 * @brief Constructor.
		 *
		 * @param[in] laszipHeader LAS header.
		 * @param[in] fileName File path.
		 *
		 * @note Check isValid() before use.
		 */
		explicit CopcLoader(const laszip_header* laszipHeader, const QString& fileName);

		/**
		 * @brief Destructor.
		 */
		~CopcLoader() = default;

		//! No copy.
		CopcLoader(CopcLoader const&)            = delete;
		CopcLoader& operator=(CopcLoader const&) = delete;

		/**
		 * @brief Set max level constraint.
		 *
		 * @param[in] maxLevelConstraint Maximum octree level.
		 */
		void setMaxLevelConstraint(uint32_t maxLevelConstraint)
		{
			m_hasMaxLevelConstraint = true;
			m_maxLevelConstraint    = maxLevelConstraint;
		}

		/**
		 * @brief Release max level constraint.
		 */
		void releaseMaxLevelConstraint()
		{
			m_hasMaxLevelConstraint = false;
			m_maxLevelConstraint    = m_maxLevel;
		}

		/**
		 * @brief Set clipping box constraint.
		 *
		 * @param[in] extent Clipping extent.
		 */
		void setClippingBoxConstraint(const LasDetails::UnscaledExtent& extent)
		{
			m_hasClippingConstraint = true;
			m_ClippingConstraint    = extent;
		}

		/**
		 * @brief Release clipping constraint.
		 */
		void releaseClippingBoxConstraint()
		{
			m_hasClippingConstraint = false;
			m_ClippingConstraint    = m_extent;
		}

		/**
		 * @brief Get point counts per level.
		 *
		 * @return Vector of point counts.
		 */
		const std::vector<uint64_t>& levelPointCounts() const
		{
			return m_levelPointCounts;
		}

		/**
		 * @brief Get full extent.
		 *
		 * @return Full (non-clipped) extent.
		 */
		const LasDetails::UnscaledExtent& extent() const
		{
			return m_extent;
		}

		/**
		 * @brief Get clipping extent.
		 *
		 * @return Current clipping extent.
		 */
		const LasDetails::UnscaledExtent& clippingExtent() const
		{
			return m_ClippingConstraint;
		}

		/**
		 * @brief Get max octree level.
		 *
		 * @return Maximum depth.
		 */
		const int32_t maxLevel() const
		{
			return m_maxLevel;
		}

		/**
		 * @brief Check if loader is valid.
		 *
		 * @return true if valid.
		 */
		bool isValid() const
		{
			return m_isValid;
		}

		/**
		 * @brief Set global shift.
		 *
		 * @param[in] globalShift Global shift vector.
		 */
		void setGlobalShift(const CCVector3d& globalShift)
		{
			m_globalShift = globalShift;
		}

		/**
		 * @brief Get chunk intervals.
		 *
		 * @param[out] sortedChunkIntervalSet Sorted chunks.
		 * @param[out] estimatedPointCount Estimated point count.
		 */
		void getChunkIntervalsSet(std::vector<std::reference_wrapper<ChunkInterval>>& sortedChunkIntervalSet, uint64_t& estimatedPointCount);

	  public: // static methods
		/**
		 * @brief Check if file is COPC.
		 *
		 * @param[in] laszipHeader LAS header.
		 *
		 * @return true if potentially COPC.
		 */
		static bool IsPutativeCOPCFile(const laszip_header* laszipHeader);

		/**
		 * @brief Check if VLR is COPC.
		 *
		 * @param[in] vlr VLR to check.
		 *
		 * @return true if COPC VLR.
		 */
		static bool IsCOPCVlr(const laszip_vlr_struct& vlr);

	  private: // methods
		/**
		 * @brief Generate chunk table hierarchy.
		 *
		 * @param[in] entries COPC entries.
		 */
		void generateChunktableIntervalsHierarchy(std::vector<Entry>& entries);

		/**
		 * @brief Check constraints on voxel.
		 *
		 * @param[in] voxelkey Voxel key.
		 *
		 * @return Filter status.
		 */
		ChunkInterval::eFilterStatus checkConstraints(const VoxelKey& voxelkey);

		/**
		 * @brief Reset interval status.
		 */
		void resetIntervalsStatus();

		/**
		 * @brief Propagate failure flag.
		 *
		 * @param[in] voxelkey Voxel key.
		 */
		void propagateFailureFlag(const VoxelKey& voxelkey);

		/**
		 * @brief Flag interval nodes.
		 *
		 * @param[in] voxelkey Voxel key.
		 *
		 * @return Point count.
		 */
		uint64_t flagIntervalNodes(const VoxelKey& voxelkey);

		/**
		 * @brief Recurse octree.
		 *
		 * @param[in] voxelkey Current voxel.
		 * @param[out] visitedNodes Visited nodes set.
		 */
		void recurse(const VoxelKey& voxelkey, std::unordered_set<VoxelKey>& visitedNodes) const;

		/**
		 * @brief Check if traversable.
		 *
		 * @return true if all nodes reachable.
		 */
		bool isTraversable() const;

		/**
		 * @brief Check for root.
		 *
		 * @return true if root exists.
		 */
		bool hasRoot() const
		{
			return m_chunkIntervalsHierarchy.count(VoxelKey::Root());
		}

	  private: // members
		//! Validity flag.
		bool m_isValid{false};

		//! Max level.
		int32_t m_maxLevel{0};

		//! Number of points.
		uint64_t m_numPoints{0};

		//! Global shift.
		CCVector3d m_globalShift;

		//! Full extent.
		UnscaledExtent m_extent;

		//! Has clipping constraint.
		bool m_hasClippingConstraint{false};

		//! Has max level constraint.
		bool m_hasMaxLevelConstraint{false};

		//! Max level constraint.
		int32_t m_maxLevelConstraint{0};

		//! Clipping extent.
		UnscaledExtent m_ClippingConstraint;

		//! Point counts per level.
		std::vector<uint64_t> m_levelPointCounts;

		//! Chunk intervals hierarchy.
		std::unordered_map<VoxelKey, ChunkInterval> m_chunkIntervalsHierarchy;

		//! COPC info.
		Info m_copcInfo;
	};
} // namespace copc
