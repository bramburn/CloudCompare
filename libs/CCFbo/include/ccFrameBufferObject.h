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
 * @file ccFrameBufferObject.h
 *
 * @brief Framebuffer object encapsulation
 *
 * OpenGL framebuffer object with color and depth attachments.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
#include "CCFbo.h"

// Qt
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_2_1>

/**
 * @brief Framebuffer object
 *
 * Encapsulates OpenGL FBO with color and depth textures.
 */
class CCFBO_LIB_API ccFrameBufferObject
{
  public:
	/**\brief Create a framebuffer object */
	ccFrameBufferObject();
	/**\brief Destructor */
	~ccFrameBufferObject();

	/**\brief Initialize FBO\param w Width\param h Height\return true on success */
	bool init(unsigned w, unsigned h);
	/**\brief Reset FBO */
	void reset();
	/**\brief Start using FBO\return true on success */
	bool start();
	/**\brief Stop using FBO */
	void stop();

	/**\brief Check if FBO is valid\return true if valid */
	inline bool isValid() const
	{
		return m_fboId;
	}

	/**\brief Initialize color texture\param internalformat GL internal format\param format GL format\param type GL type\param minMagFilter Min/mag filter\param target GL target\return true on success */
	bool initColor(GLint internalformat = GL_RGBA,
	               GLenum format = GL_RGBA,
	               GLenum type = GL_UNSIGNED_BYTE,
	               GLint minMagFilter = GL_NEAREST,
	               GLenum target = GL_TEXTURE_2D);

	/**\brief Attach color texture\param texID Texture ID\param ownTexture Whether FBO owns texture\param target GL target\return true on success */
	bool attachColor(GLuint texID,
	                 bool ownTexture = false,
	                 GLenum target = GL_TEXTURE_2D);

	/**\brief Initialize depth texture\param wrapParam Wrap parameter\param internalFormat Internal format\param minMagFilter Min/mag filter\param textureTarget GL target\return true on success */
	bool initDepth(GLint wrapParam = GL_CLAMP_TO_BORDER,
	               GLenum internalFormat = GL_DEPTH_COMPONENT32F,
	               GLint minMagFilter = GL_NEAREST,
	               GLenum textureTarget = GL_TEXTURE_2D);

	/**\brief Attach depth texture\param texID Texture ID\param ownTexture Whether FBO owns texture\param target GL target\return true on success */
	bool attachDepth(GLuint texID,
	                 bool ownTexture = false,
	                 GLenum target = GL_TEXTURE_2D);

	/// Get FBO ID
	inline GLuint getID() const
	{
		return m_fboId;
	}
	/// Get color texture ID
	inline GLuint getColorTexture() const
	{
		return m_colorTexture;
	}
	/// Get depth texture ID
	inline GLuint getDepthTexture() const
	{
		return m_depthTexture;
	}

	/// Get width
	inline unsigned width() const
	{
		return m_width;
	}
	/// Get height
	inline unsigned height() const
	{
		return m_height;
	}

  protected: // methods
	//! Deletes/releases the color texture
	void deleteColorTexture();

	//! Deletes/releases the depth texture
	void deleteDepthTexture();

  protected: // members
	//! FBO validity
	bool m_isValid;

	//! Width
	unsigned m_width;
	//! Height
	unsigned m_height;

	//! Depth texture GL ID
	GLuint m_depthTexture;

	//! Whether the depth texture is owned by this FBO or not
	bool m_ownDepthTexture;

	//! Color texture GL ID
	GLuint m_colorTexture;

	//! Whether the color texture is owned by this FBO or not
	bool m_ownColorTexture;

	//! ID
	GLuint m_fboId;

	// For portability, we need to use 2.1...
	QOpenGLFunctions_2_1 m_glFunc;

	//... and QOpenGLExtraFunctions
	QOpenGLExtraFunctions m_glExtFunc;
};
