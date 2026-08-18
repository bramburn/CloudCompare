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

#ifndef CC_COLOR_RAMP_SHADER_HEADER
#define CC_COLOR_RAMP_SHADER_HEADER

/**
 * @file ccColorRampShader.h
 *
 * @brief Color ramp shader
 *
 * GLSL shader for color ramp rendering.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Always on top!
#include "ccIncludeGL.h"

// CCFbo
#include <ccShader.h>

// Local
#include "ccColorScale.h"

/**
 * @brief Color ramp shader
 *
 * GLSL shader for rendering color ramps.
 */
class QCC_DB_LIB_API ccColorRampShader : public ccShader
{
	Q_OBJECT

  public:
	/**
	 * @brief Create shader
	 */
	ccColorRampShader();

	/**
	 * @brief Destructor
	 */
	virtual ~ccColorRampShader()
	{
	}

	/**
	 * @brief Setup shader
	 * @param[in] glFunc OpenGL functions
	 * @param[in] minSatRel Min saturation
	 * @param[in] maxSatRel Max saturation
	 * @param[in] colorSteps Number of color steps
	 * @param[in] colorScale Color scale to use
	 * @return true on success
	 */
	bool setup(QOpenGLFunctions_2_1* glFunc, float minSatRel, float maxSatRel, unsigned colorSteps, const ccColorScale::Shared& colorScale);

	/// Get maximum color ramp size
	static unsigned MaxColorRampSize();

	/// Get minimum required shader memory
	static GLint MinRequiredBytes();
};

#endif // CC_COLOR_RAMP_SHADER_HEADER
