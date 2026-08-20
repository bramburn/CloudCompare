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

#ifndef CC_RESERVED_IDS_HEADER
#define CC_RESERVED_IDS_HEADER

/**
 * @file ccReservedIDs.h
 *
 * @brief Reserved unique IDs for special entities in CloudCompare.
 *
 * @details Defines unique IDs that are reserved for special built-in
 * entities such as graphical tools and overlays. Plugins and user code
 * should never use these IDs to avoid conflicts.
 *
 * These IDs are used by ccHObject::getUniqueID() to ensure that
 * certain critical entities always have predictable ID values.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccHObject
 */

/**
 * @enum ReservedIDs
 *
 * @brief Unique IDs reserved for special built-in entities.
 *
 * @details These IDs are reserved for CloudCompare's internal use.
 * They ensure that certain graphical entities (like clipping planes,
 * segmentation tools, and polyline tools) always have consistent,
 * well-known IDs.
 *
 * @note Plugins and user code should never assign or use these IDs.
 *       Use ccHObject::getUniqueID() to generate new unique IDs.
 *
 * @var ReservedIDs::CLIPPING_BOX
 * @brief The clipping box entity.
 *
 * @var ReservedIDs::INTERACTIVE_SEGMENTATION_TOOL_POLYLINE
 * @brief The polyline used by the interactive segmentation tool.
 *
 * @var ReservedIDs::INTERACTIVE_SEGMENTATION_TOOL_POLYLINE_VERTICES
 * @brief The vertices of the segmentation tool polyline.
 *
 * @var ReservedIDs::TRACE_POLYLINE_TOOL_POLYLINE_TIP
 * @brief The tip/endpoint of the trace polyline tool.
 *
 * @var ReservedIDs::TRACE_POLYLINE_TOOL_POLYLINE_TIP_VERTICES
 * @brief The vertices of the trace polyline tip.
 *
 * @var ReservedIDs::TRACE_POLYLINE_TOOL_POLYLINE
 * @brief The main polyline of the trace polyline tool.
 *
 * @var ReservedIDs::TRACE_POLYLINE_TOOL_POLYLINE_VERTICES
 * @brief The vertices of the trace polyline.
 */
enum class ReservedIDs : unsigned
{
	CLIPPING_BOX                                    = 1,
	INTERACTIVE_SEGMENTATION_TOOL_POLYLINE          = 2,
	INTERACTIVE_SEGMENTATION_TOOL_POLYLINE_VERTICES = 3,
	TRACE_POLYLINE_TOOL_POLYLINE_TIP                = 4,
	TRACE_POLYLINE_TOOL_POLYLINE_TIP_VERTICES       = 5,
	TRACE_POLYLINE_TOOL_POLYLINE                    = 6,
	TRACE_POLYLINE_TOOL_POLYLINE_VERTICES           = 7,
};

#endif // CC_RESERVED_IDS_HEADER
