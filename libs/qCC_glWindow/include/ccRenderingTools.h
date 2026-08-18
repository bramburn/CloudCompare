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
 * @file ccRenderingTools.h
 *
 * @brief Rendering tools
 *
 * Miscellaneous rendering utilities.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "qCC_glWindow.h"

// qCC_db
#include <ccDrawableObject.h>

class QWidget;
class ccGBLSensor;
class ccScalarField;
class ccGLWindowInterface;

/**
 * @brief Rendering tools
 *
 * Miscellaneous rendering utilities.
 */
class CCGLWINDOW_LIB_API ccRenderingTools
{
  public:
	/**
	 * @brief Show depth buffer
	 * @param[in] lidar GBL sensor
	 * @param[in] parent Parent widget
	 * @param[in] maxDim Max display dimension
	 */
	static void ShowDepthBuffer(ccGBLSensor* lidar, QWidget* parent = nullptr, unsigned maxDim = 1024);

	/**
	 * @brief Draw color ramp
	 * @param[in] context OpenGL context
	 */
	static void DrawColorRamp(const CC_DRAW_CONTEXT& context);

	/**
	 * @brief Draw color ramp for scalar field
	 * @param[in] context OpenGL context
	 * @param[in] sf Scalar field
	 * @param[in] win GL window
	 * @param[in] glW Window width
	 * @param[in] glH Window height
	 * @param[in] renderZoom Render zoom
	 */
	static void DrawColorRamp(const CC_DRAW_CONTEXT& context, const ccScalarField* sf, ccGLWindowInterface* win, int glW, int glH, float renderZoom = 1.0f);
};
