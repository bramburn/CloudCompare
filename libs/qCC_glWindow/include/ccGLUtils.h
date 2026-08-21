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
 * @brief OpenGL utilities and view orientation helpers
 *
 * Provides:
 * - OpenGL texture display functions (QImage or GLuint-based)
 * - View matrix generation from orientation constants or rotation angles
 * - View orientation constants (TOP, FRONT, ISO, etc.)
 *
 * Texture display: supports full-screen or split-screen modes
 * (HalfLeft / HalfRight) for stereo/coupled viewport rendering.
 *
 * View matrices: GenerateViewMat() creates a 4×4 view matrix from
 * a CC_VIEW_ORIENTATION or from vertical/orthogonal rotation angles.
 * Used by ccGLWindow for viewport switching (View > Standard Views menu).
 *
 * @see ccGLMatrix for the matrix type
 */

#include "qCC_glWindow.h"
#include <ccIncludeGL.h> // Always first!
#include <QImage>

/**
 * @brief Standard view orientations for the 3D viewport
 *
 * Used by View > Standard Views menu and GenerateViewMat().
 * ISO views are the two standard isometric orientations.
 */
enum CC_VIEW_ORIENTATION
{
	CC_TOP_VIEW,      //!< Top view (eye: +Z, looking down)
	CC_BOTTOM_VIEW,   //!< Bottom view (eye: -Z)
	CC_FRONT_VIEW,    //!< Front view (eye: -Y)
	CC_BACK_VIEW,     //!< Back view (eye: +Y)
	CC_LEFT_VIEW,     //!< Left view (eye: -X)
	CC_RIGHT_VIEW,    //!< Right view (eye: +X)
	CC_ISO_VIEW_1,   //!< Isometric view 1 (standard 45°/35.264°)
	CC_ISO_VIEW_2,   //!< Isometric view 2 (rotated 90° from ISO_VIEW_1)
};

/**
 * @class ccGLUtils
 *
 * @brief OpenGL utility functions
 *
 * Static helpers for texture rendering and view matrix construction.
 */
class CCGLWINDOW_LIB_API ccGLUtils
{
  public:
	/**
	 * @brief Texture display area for split-screen modes
	 *
	 * Used in stereo/coupled viewport rendering.
	 */
	enum TextureArea
	{
		Full,      //!< Full texture
		HalfLeft,  //!< Left half only
		HalfRight  //!< Right half only
	};

	/**
	 * @brief Display a QImage as a 2D texture at a position
	 *
	 * @param[in] image Texture image
	 * @param[in] x Left position
	 * @param[in] y Bottom position
	 * @param[in] w Width in pixels
	 * @param[in] h Height in pixels
	 * @param[in] alpha Global alpha
	 * @param[in] area Which half to display (Full/HalfLeft/HalfRight)
	 */
	static void DisplayTexture2DPosition(QImage image,
	                                    int     x,
	                                    int     y,
	                                    int     w,
	                                    int     h,
	                                    unsigned char alpha   = 255,
	                                    TextureArea          = Full);

	/**
	 * @brief Display a QImage as a centered 2D texture
	 *
	 * @param[in] image Texture image
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Global alpha
	 */
	inline static void DisplayTexture2D(QImage image, int w, int h, unsigned char alpha = 255)
	{
		DisplayTexture2DPosition(image, -w / 2, -h / 2, w, h, alpha);
	}

	/**
	 * @brief Display an OpenGL texture by ID at a position
	 *
	 * @param[in] texID OpenGL texture name (from glGenTextures)
	 * @param[in] x Left position
	 * @param[in] y Bottom position
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Global alpha
	 * @param[in] area Which half to display
	 */
	static void DisplayTexture2DPosition(GLuint           texID,
	                                    int              x,
	                                    int              y,
	                                    int              w,
	                                    int              h,
	                                    unsigned char     alpha   = 255,
	                                    TextureArea       area    = Full);

	/**
	 * @brief Display an OpenGL texture by ID, centered
	 *
	 * @param[in] texID OpenGL texture name
	 * @param[in] w Width
	 * @param[in] h Height
	 * @param[in] alpha Global alpha
	 */
	inline static void DisplayTexture2D(GLuint texID, int w, int h, unsigned char alpha = 255)
	{
		DisplayTexture2DPosition(texID, -w / 2, -h / 2, w, h, alpha);
	}

	/*** View matrices ***/

	/**
	 * @brief Generate a view matrix for a standard orientation
	 *
	 * Creates a 4×4 view matrix that positions the camera to look at
	 * the scene from a standard orientation. Optionally returns the
	 * vertical and orthogonal rotation angles.
	 *
	 * @param[in] orientation Standard orientation (TOP, ISO_VIEW_1, etc.)
	 * @param[in] vertDir Vertical direction (default +Z)
	 * @param[out] _vertAngle_rad Vertical rotation angle (optional)
	 * @param[out] _orthoAngle_rad Orthogonal rotation angle (optional)
	 * @return 4×4 view matrix
	 */
	static ccGLMatrixd GenerateViewMat(CC_VIEW_ORIENTATION orientation,
	                                   const CCVector3d&   vertDir         = CCVector3d(0, 0, 1),
	                                   double*             _vertAngle_rad  = nullptr,
	                                   double*             _orthoAngle_rad = nullptr);

	/**
	 * @brief Generate a view matrix from rotation angles
	 *
	 * @param[in] vertDir Vertical direction
	 * @param[in] vertAngle_rad Rotation around vertDir
	 * @param[in] orthoAngle_rad Rotation around orthogonal axis
	 * @return 4×4 view matrix
	 */
	static ccGLMatrixd GenerateViewMat(const CCVector3d& vertDir,
	                                   double            vertAngle_rad,
	                                   double            orthoAngle_rad);
};
