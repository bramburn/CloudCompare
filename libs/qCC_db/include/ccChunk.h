// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        //
// ##########################################################################

/**
 * @file ccChunk.h
 *
 * @brief Chunk management utilities for large array handling.
 *
 * @details Provides static methods for managing chunked arrays with
 * fixed-size chunks for efficient memory handling.
 *
 * ## Overview
 *
 * CloudCompare uses 64KB chunks (2^16 elements) to:
 * - Efficiently handle large arrays
 * - Manage memory in fixed-size blocks
 * - Support memory-mapped file operations
 *
 * ## Chunk Size
 *
 * Each chunk is 64KB = 65,536 bytes = 2^16 elements.
 *
 * This size is chosen for:
 * - Cache line alignment
 * - Memory allocation efficiency
 * - Memory-mapped file support
 *
 * ## Usage
 *
 * @code
 * // Count chunks needed
 * size_t chunks = ccChunk::Count(150000);
 *
 * // Get chunk size
 * size_t size = ccChunk::Size(2, 150000);
 *
 * // Get start position
 * size_t pos = ccChunk::StartPos(1);
 *
 * // Get pointer to chunk start
 * auto* data = ccChunk::Start(buffer, 0);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_CHUNK_HEADER
#define CC_CHUNK_HEADER

// System
#include <vector>

/**
 * @brief Chunk management utilities.
 *
 * @details Static methods for chunked array operations.
 *
 * All methods operate on 64KB fixed-size chunks.
 */
class ccChunk
{
  public:
	//! Chunk size power of 2 (16 = 65536).
	static const size_t SIZE_POWER = 16;

	//! Chunk size in elements (64KB).
	static const size_t SIZE = (1 << SIZE_POWER);

	/**
	 * @brief Count chunks needed for elements.
	 *
	 * @param[in] elementCount Total elements.
	 *
	 * @return Number of chunks needed.
	 */
	inline static size_t Count(size_t elementCount)
	{
		return (elementCount >> SIZE_POWER) + ((elementCount & (SIZE - 1)) ? 1 : 0);
	}

	/**
	 * @brief Get chunk size.
	 *
	 * @param[in] chunkIndex Chunk index.
	 * @param[in] elementCount Total elements.
	 *
	 * @return Elements in this chunk.
	 */
	inline static size_t Size(size_t chunkIndex, size_t elementCount)
	{
		return (chunkIndex + 1 < Count(elementCount) ? SIZE : elementCount - chunkIndex * SIZE);
	}

	/**
	 * @brief Get chunk size.
	 *
	 * @param[in] chunkIndex Chunk index.
	 * @param[in] chunkCount Total chunks.
	 * @param[in] elementCount Total elements.
	 *
	 * @return Elements in this chunk.
	 */
	inline static size_t Size(size_t chunkIndex, size_t chunkCount, size_t elementCount)
	{
		return (chunkIndex + 1 < chunkCount ? SIZE : elementCount - chunkIndex * SIZE);
	}

	/**
	 * @brief Get chunk start position.
	 *
	 * @param[in] chunkIndex Chunk index.
	 *
	 * @return Element offset.
	 */
	inline static size_t StartPos(size_t chunkIndex)
	{
		return chunkIndex * SIZE;
	}

	/**
	 * @brief Get pointer to chunk start.
	 *
	 * @tparam T Buffer element type.
	 * @param[in] buffer Buffer vector.
	 * @param[in] chunkIndex Chunk index.
	 *
	 * @return Pointer to chunk start.
	 */
	template <typename T>
	inline static T* Start(std::vector<T>& buffer, size_t chunkIndex)
	{
		return buffer.data() + StartPos(chunkIndex);
	}

	/**
	 * @brief Get pointer to chunk start (const).
	 *
	 * @tparam T Buffer element type.
	 * @param[in] buffer Buffer vector.
	 * @param[in] chunkIndex Chunk index.
	 *
	 * @return Pointer to chunk start.
	 */
	template <typename T>
	inline static const T* Start(const std::vector<T>& buffer, size_t chunkIndex)
	{
		return buffer.data() + StartPos(chunkIndex);
	}

	/**
	 * @brief Count chunks in buffer.
	 *
	 * @tparam T Buffer element type.
	 * @param[in] buffer Buffer vector.
	 *
	 * @return Number of chunks.
	 */
	template <typename T>
	inline static size_t Count(const std::vector<T>& buffer)
	{
		return Count(buffer.size());
	}

	/**
	 * @brief Get chunk size for buffer.
	 *
	 * @tparam T Buffer element type.
	 * @param[in] chunkIndex Chunk index.
	 * @param[in] buffer Buffer vector.
	 *
	 * @return Elements in this chunk.
	 */
	template <typename T>
	inline static size_t Size(size_t chunkIndex, const std::vector<T>& buffer)
	{
		return Size(chunkIndex, buffer.size());
	}
};

#endif // CC_CHUNK_HEADER
