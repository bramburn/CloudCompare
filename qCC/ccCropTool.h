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
 * @brief Crop tool
 *
 * Cropping tool for clouds and meshes.
 */

// qCC_db
#include <ccBBox.h>

class ccHObject;
class ccGLMatrix;

/**
 * @class ccCropTool
 *
 * @brief Cropping tool
 *
 * Handles cropping of clouds and meshes.
 */
class ccCropTool
{
  public:
	/**
	 * @brief Crop entity
	 *
	 * Crops the input entity.
	 *
	 * @param[in] entity Entity to be cropped (cloud or mesh)
	 * @param[in] box Cropping box
	 * @param[in] inside Keep points inside (true) or outside (false)
	 * @param[in] meshRotation Optional rotation for meshes
	 * @return Cropped entity (if any)
	 */
	static ccHObject* Crop(ccHObject* entity, const ccBBox& box, bool inside = true, const ccGLMatrix* meshRotation = nullptr);
};

#endif // CC_CROP_TOOL_HEADER
