#pragma once
// ##########################################################################
// #                                                                        #
// #                               CCFBO                                    #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU Library General Public License as       #
// #  published by the Free Software Foundation; version 2 or later of the License.  #
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
 * @file ccShader.h
 *
 * @brief GLSL shader program wrapper
 *
 * Encapsulates QOpenGLShaderProgram with helper functions
 * for loading vertex and fragment shaders from files.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "CCFbo.h"

// Qt
#include <QOpenGLShaderProgram>
#include <QString>

class QObject;

/**
 * @brief GLSL shader program
 *
 * Wrapper around QOpenGLShaderProgram for loading
 * and managing GLSL shaders.
 */
class CCFBO_LIB_API ccShader : public QOpenGLShaderProgram
{
	Q_OBJECT

  public:
	/**
	 * @brief Create a shader program
	 * @param[in] parent Parent object
	 */
	ccShader(QObject* parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~ccShader() = default;

	/**
	 * @brief Load shader from file (auto .vert/.frag)
	 * @param[in] shaderBasePath Path to shader files
	 * @param[in] shaderBaseFilename Base filename (no extension)
	 * @param[out] error Error message if failed
	 * @return true on success
	 */
	virtual bool fromFile(QString shaderBasePath, QString shaderBaseFilename, QString& error);

	/**
	 * @brief Load shader from explicit files
	 * @param[in] vertShaderFile Vertex shader path
	 * @param[in] fragShaderFile Fragment shader path
	 * @param[out] error Error message if failed
	 * @return true on success
	 */
	virtual bool loadProgram(QString vertShaderFile, QString fragShaderFile, QString& error);
};
