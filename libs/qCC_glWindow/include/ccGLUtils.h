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
 * @file ccGLUtils.h
 *
 * @brief OpenGL utilities and view orientation constants
 *
 * Provides OpenGL texture display, matrix generation,
 * and view orientation helpers.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "qCC_glWindow.h"

// qCC_db
#include <ccIncludeGL.h> //Always first!

// Qt
#include <QImage>

/// View orientation for 3D views
enum CC_VIEW_ORIENTATION
{
	CC_TOP_VIEW,    //!< Top view (eye: +Z)
	CC_BOTTOM_VIEW, //!< Bottom view
	CC_FRONT_VIEW,  //!< Front view
	CC_BACK_VIEW,   //!< Back view
	CC_LEFT_VIEW,   //!< Left view
	CC_RIGHT_VIEW,  //!< Right view
	CC_ISO_VIEW_1,  //!< Isometric view 1
	CC_ISO_VIEW_2,  //!< Isometric view 2
};

/**
 * @brief OpenGL utilities
 */
class CCGLWINDOW_LIB_API ccGLUtils
{
  public:
	/// Texture display area
	enum TextureArea
	{
		Full,
		HalfLeft,
		HalfRight
	};

	/**
	 * @brief Display 2D texture at position
	 * @param[in] image Texture image
	 * @param[in] x X position
	 * @param[in] y Y position
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Alpha value
	 * @param[in] area Texture area
	 */
	static void        DisplayTexture2DPosition(QImage image, int x, int y, int w, int h, unsigned char alpha = 255, TextureArea area = Full);
	
	/**
	 * @brief Display 2D texture (centered)
	 * @param[in] image Texture image
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Alpha value
	 */
	inline static void DisplayTexture2D(QImage image, int w, int h, unsigned char alpha = 255)
	{
		DisplayTexture2DPosition(image, -w / 2, -h / 2, w, h, alpha);
	}

	/**
	 * @brief Display texture by ID at position
	 * @param[in] texID OpenGL texture ID
	 * @param[in] x X position
	 * @param[in] y Y position
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Alpha value
	 * @param[in] area Texture area
	 */
	static void        DisplayTexture2DPosition(GLuint texID, int x, int y, int w, int h, unsigned char alpha = 255, TextureArea area = Full);
	
	/**
	 * @brief Display texture by ID (centered)
	 * @param[in] texID OpenGL texture ID
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Alpha value
	 */
	inline static void DisplayTexture2D(GLuint texID, int w, int h, unsigned char alpha = 255)
	{
		DisplayTexture2DPosition(texID, -w / 2, -h / 2, w, h, alpha);
	}

	/***************************************************
	                OpenGL Matrices
	***************************************************/

	/**
	 * @brief Generate view matrix for orientation
	 * @param[in] orientation View orientation
	 * @param[in] vertDir Vertical direction
	 * @param[out] _vertAngle_rad Vertical rotation angle
	 * @param[out] _orthoAngle_rad Orthogonal rotation angle
	 * @return View matrix
	 */
	static ccGLMatrixd GenerateViewMat(CC_VIEW_ORIENTATION orientation,
	                                   const CCVector3d&   vertDir         = CCVector3d(0, 0, 1),
	                                   double*             _vertAngle_rad  = nullptr,
	                                   double*             _orthoAngle_rad = nullptr);

	/**
	 * @brief Generate view matrix from angles
	 * @param[in] vertDir Vertical direction
	 * @param[in] vertAngle_rad Vertical rotation
	 * @param[in] orthoAngle_rad Orthogonal rotation
	 * @return View matrix
	 */
	static ccGLMatrixd GenerateViewMat(const CCVector3d& vertDir,
	                                   double            vertAngle_rad,
	                                   double            orthoAngle_rad);
};
