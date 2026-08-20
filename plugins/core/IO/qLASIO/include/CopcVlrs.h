/**
 * @file CopcVlrs.h
 *
 * @brief COPC VLR structures and helper classes.
 *
 * @details Defines COPC (Cloud Optimized Point Cloud) VLR structures
 * including Info, VoxelKey, Entry, and Page.
 *
 * ## Structures
 *
 * - **Info**: COPC file header info
 * - **VoxelKey**: Octree voxel key (level + xyz)
 * - **Entry**: Octree node entry
 * - **Page**: Hierarchy page
 *
 * ## Voxel Key Format
 *
 * VoxelKey encodes octree position:
 * - level: depth in octree
 * - x, y, z: cell coordinates at that level
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
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Hobu, Inc.                           	    #
// #                                                                        //
// ##########################################################################

// qLASIO
#include "LasDetails.h"

// CCCoreLib
#include <CCGeom.h>

// Qt
#include <QDataStream>

// System
#include <array>
#include <cstddef>
#include <cstdint>

/**
 * @brief COPC support namespace.
 */
namespace copc
{
	/**
	 * @brief COPC file information.
	 */
	struct Info
	{
		/**
		 * @brief Extract extent from info.
		 *
		 * @param[out] extent Extent to populate.
		 *
		 * @return true if valid.
		 */
		bool extractExtent(LasDetails::UnscaledExtent& extent) const
		{
			extent.clear();
			CCVector3d center(center_x, center_y, center_z);
			CCVector3d halfsizeVec(halfsize, halfsize, halfsize);
			extent.add(center + halfsizeVec);
			extent.add(center - halfsizeVec);
			return extent.isValid() && extent.getDiagNormd() > 0;
		}

		//! Serialization.
		friend QDataStream& operator>>(QDataStream& stream, Info& copc_info)
		{
			quint64 root_hier_offset_, root_hier_size_, reserved_;
			stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
			stream >> copc_info.center_x >> copc_info.center_y >> copc_info.center_z;
			stream >> copc_info.halfsize >> copc_info.spacing;
			stream >> root_hier_offset_ >> root_hier_size_;
			copc_info.root_hier_offset = root_hier_offset_;
			copc_info.root_hier_size   = root_hier_size_;
			stream >> copc_info.gpstime_minimum >> copc_info.gpstime_minimum;
			std::for_each(std::begin(copc_info.reserved), std::end(copc_info.reserved), [&stream, &reserved_](auto& reserved)
			              { stream >> reserved_; });
			return stream;
		};

		//! Structure size.
		static constexpr size_t SIZE = 160;

		//! Center X.
		double center_x{0.0};
		//! Center Y.
		double center_y{0.0};
		//! Center Z.
		double center_z{0.0};
		//! Half size (extent / 2).
		double halfsize{0.0};
		//! Spacing at root level.
		double spacing{0.0};

		//! Root hierarchy offset.
		uint64_t root_hier_offset{0};
		//! Root hierarchy size.
		uint64_t root_hier_size{0};

		//! GPS time minimum.
		double gpstime_minimum{0.0};
		//! GPS time maximum.
		double gpstime_maximum{0.0};

		//! Reserved (should be 0).
		uint64_t reserved[11] = {0};
	};

	/**
	 * @brief Octree voxel key.
	 *
	 * Identifies a cell in the COPC octree.
	 */
	struct VoxelKey
	{
		/**
		 * @brief Create invalid key.
		 */
		static VoxelKey Invalid()
		{
			return {-1, 0, 0, 0};
		}

		/**
		 * @brief Create root key.
		 */
		static VoxelKey Root()
		{
			return {0, 0, 0, 0};
		}

		/**
		 * @brief Check if valid.
		 */
		bool isValid() const
		{
			const int32_t numCellInOneDim = std::pow(2, level);
			return level >= 0 && x >= 0 && y >= 0 && z >= 0 && z < numCellInOneDim && y < numCellInOneDim && x < numCellInOneDim;
		}

		/**
		 * @brief Extract voxel extent.
		 *
		 * @param[in] rootExtent Root extent.
		 * @param[out] voxelExtent Voxel extent.
		 *
		 * @return true if valid.
		 */
		bool extractExtent(const LasDetails::UnscaledExtent& rootExtent, LasDetails::UnscaledExtent& voxelExtent) const;

		/**
		 * @brief Generate children keys.
		 *
		 * @return Array of 8 child keys.
		 */
		std::array<VoxelKey, 8> childrenKeys() const
		{
			std::array<VoxelKey, 8> children{};
			for (int32_t i = 0; i < 8; i++)
			{
				children[i].level = level + 1;
				children[i].x     = (x << 1) | (i & 0x1);
				children[i].y     = (y << 1) | ((i >> 1) & 0x1);
				children[i].z     = (z << 1) | ((i >> 2) & 0x1);
			}
			return children;
		}

		/**
		 * @brief Get parent key.
		 */
		VoxelKey parentKey() const
		{
			if (!isValid() || level == 0)
			{
				return VoxelKey::Invalid();
			}
			return {level - 1, x >> 1, y >> 1, z >> 1};
		}

		//! Equality operator.
		bool operator==(const VoxelKey& other) const
		{
			return other.level == level && other.x == x && other.y == y && other.z == z;
		};

		/**
		 * @brief Check if parent of other key.
		 */
		bool isParent(const VoxelKey& other) const
		{
			if (!other.isValid())
				return false;
			return other.parentKey() == *this;
		}

		//! Serialization.
		friend QDataStream& operator>>(QDataStream& stream, VoxelKey& key)
		{
			stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
			stream >> key.level >> key.x >> key.y >> key.z;
			return stream;
		};

		//! Structure size.
		static constexpr size_t SIZE = 16;

		//! Octree level.
		int32_t level{0};
		//! X coordinate.
		int32_t x{0};
		//! Y coordinate.
		int32_t y{0};
		//! Z coordinate.
		int32_t z{0};
	};

	/**
	 * @brief Octree node entry.
	 */
	struct Entry
	{
		/**
		 * @brief Check if hierarchy page.
		 *
		 * @return true if point_count < 0.
		 */
		bool isHierarchyPage() const
		{
			return point_count < 0;
		}

		//! Serialization.
		friend QDataStream& operator>>(QDataStream& stream, Entry& entry)
		{
			stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
			quint64 offset_;
			stream >> entry.key >> offset_ >> entry.byte_size >> entry.point_count;
			entry.offset = offset_;
			return stream;
		};

		//! Structure size.
		static constexpr size_t SIZE = VoxelKey::SIZE + 16;

		//! Voxel key.
		VoxelKey key;
		//! Byte offset in file.
		uint64_t offset{0};
		//! Byte size of data.
		int32_t byte_size{0};
		//! Point count (-1 = hierarchy page, 0 = no points).
		int32_t point_count{0};
	};

	/**
	 * @brief Hierarchy page.
	 */
	struct Page
	{
		/**
		 * @brief Constructor.
		 *
		 * @param[in] offset_ File offset.
		 * @param[in] num_entries_ Number of entries.
		 */
		Page(const uint64_t offset_, const size_t num_entries_)
		    : offset(offset_)
		    , num_entries(num_entries_)
		{
		}

		//! Destructor.
		~Page() = default;

		//! File offset.
		uint64_t offset{0};
		//! Number of entries.
		size_t num_entries{0};
	};
} // namespace copc

namespace std
{
	//! Hash function for VoxelKey.
	template <>
	struct hash<copc::VoxelKey>
	{
		std::size_t operator()(copc::VoxelKey const& key) const noexcept
		{
			std::hash<uint64_t> h;

			uint64_t k1 = (static_cast<uint64_t>(key.level) << 32) | key.x;
			uint64_t k2 = (static_cast<uint64_t>(key.y) << 32) | key.z;
			return h(k1) ^ (h(k2) << 1);
		}
	};
} // namespace std
