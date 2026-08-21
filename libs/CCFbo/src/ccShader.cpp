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
 * @file ccShader.cpp
 *
 * @brief GLSL shader wrapper implementation
 *
 * Wraps OpenGL GLSL shader compilation and linking:
 * - Compiles vertex and fragment shaders from source strings
 * - Links shaders into a program object
 * - Binds/unbinds the program for rendering
 * - Provides uniform setter methods (float, int, vec, mat)
 *
 * ## Usage
 *
 * 1. Create ccShader
 * 2. setShaders(vertSrc, fragSrc)
 * 3. compile()
 * 4. bind() before drawing
 * 5. setUniform*(...) to update uniforms
 * 6. release() after drawing
 *
 * @see ccShader.h
 */
#include "ccShader.h"

// Qt
#include <QFile>

// system
#include <assert.h>
#include <stdio.h>
#include <string.h>

ccShader::ccShader(QObject* parent /*=nullptr*/)
    : QOpenGLShaderProgram(parent)
{
}

bool ccShader::fromFile(QString shaderBasePath, QString shaderBaseFilename, QString& error)
{
	if (shaderBasePath.isEmpty() || shaderBaseFilename.isEmpty())
	{
		error = "Missing input argument for ccShader::fromFile";
		return false;
	}

	QString vertFilename = QString("%1/%2.vert").arg(shaderBasePath, shaderBaseFilename);
	QString fragFilename = QString("%1/%2.frag").arg(shaderBasePath, shaderBaseFilename);

	return loadProgram(vertFilename, fragFilename, error);
}

bool ccShader::loadProgram(QString vertexShaderFile, QString fragShaderFile, QString& error)
{
	if (!vertexShaderFile.isEmpty() && !addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile))
	{
		error = log();
		return false;
	}

	if (!fragShaderFile.isEmpty() && !addShaderFromSourceFile(QOpenGLShader::Fragment, fragShaderFile))
	{
		error = log();
		return false;
	}

	if (!link())
	{
		error = log();
		return false;
	}

	return true;
}
