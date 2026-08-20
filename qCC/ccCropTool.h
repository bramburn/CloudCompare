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

#ifndef CC_CROP_TOOL_HEADER
#define CC_CROP_TOOL_HEADER

/**
 * @file ccCropTool.h
 *
 * @brief Cropping tool for point clouds and meshes.
 *
 * @details Provides a static utility class for cropping geometric entities
 * using an axis-aligned bounding box (AABB). Supports both "inside" and
 * "outside" cropping modes.
 *
 * For point clouds, cropping uses ccPointCloud::crop() which creates
 * a ReferenceCloud of points within the box.
 *
 * For meshes, cropping uses CCCoreLib::ManualSegmentationTools::segmentMeshWithAABox()
 * which cuts triangles at the box boundaries and generates a new mesh
 * containing only the desired portion.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPointCloud
 * @see ccGenericMesh
 * @see ccBBox
 */

// qCC_db
#include <ccBBox.h>

class ccHObject;
class ccGLMatrix;

/**
 * @brief Cropping tool for point clouds and meshes.
 *
 * @details A static utility class providing the Crop() method to extract
 * a portion of a geometric entity using an axis-aligned bounding box.
 *
 * Supported entity types:
 * - Point clouds (ccPointCloud)
 * - Meshes (ccGenericMesh)
 *
 * Cropping modes:
 * - Inside: keeps points/triangles inside the box
 * - Outside: keeps points/triangles outside the box
 *
 * The tool preserves associated data on points/vertices:
 * - Colors
 * - Scalar fields
 * - Materials
 * - Normals
 *
 * @note For meshes, an optional rotation matrix can be provided to
 *       transform vertices before cropping, allowing for rotated
 *       crop regions.
 *
 * @par Usage
 * @code
 * // Crop a point cloud to a box
 * ccBBox cropBox(minCorner, maxCorner);
 * ccHObject* cropped = ccCropTool::Crop(cloud, cropBox, true);
 *
 * // Crop a mesh (keeping outside)
 * ccHObject* exterior = ccCropTool::Crop(mesh, cropBox, false);
 * @endcode
 */
class ccCropTool
{
  public:
	/**
	 * @brief Crop an entity using a bounding box.
	 *
	 * @param[in] entity Entity to crop (point cloud or mesh).
	 * @param[in] box Cropping bounding box (axis-aligned).
	 * @param[in] inside If true, keep points/triangles inside the box;
	 *                   if false, keep points/triangles outside.
	 * @param[in] meshRotation Optional rotation matrix to apply to mesh
	 *                         vertices before cropping (for rotated crop regions).
	 *
	 * @return Cropped entity, or nullptr on failure.
	 *
	 * @details For point clouds:
	 * - Creates a ReferenceCloud of points within the box
	 * - Returns a partial clone of the original cloud
	 * - Preserves colors and scalar fields on retained points
	 *
	 * @details For meshes:
	 * - Cuts triangles at box boundaries
	 * - Generates a new mesh with the appropriate portion
	 * - Preserves vertex colors, scalar fields, materials, and normals
	 * - If meshRotation is provided, vertices are transformed before
	 *   cropping and the result is transformed back
	 *
	 * @note The returned entity is independent and must be managed
	 *       by the caller (added to DB tree, deleted, etc.)
	 */
	static ccHObject* Crop(ccHObject* entity, const ccBBox& box, bool inside = true, const ccGLMatrix* meshRotation = nullptr);
};

#endif // CC_CROP_TOOL_HEADER
