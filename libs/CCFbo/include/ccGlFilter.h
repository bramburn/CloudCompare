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

#include "CCFbo.h"

/**
 * @file ccGlFilter.h
 *
 * @brief OpenGL filter interface
 *
 * Base interface for post-processing filters applied to
 * the 3D scene using framebuffer objects (FBOs) and shaders.
 * Examples: SSAO, EDL, etc.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Qt
#include <QString>

/**
 * @brief OpenGL post-processing filter
 *
 * Shader-based filter applied to the rendered scene.
 */
class CCFBO_LIB_API ccGlFilter
{
  public:
	/**
	 * @brief Create a filter
	 * @param[in] description Filter description
	 */
	ccGlFilter(QString description)
	    : m_isValid(false)
	    , m_description(description)
	{
	}

	/**
	 * @brief Destructor
	 */
	virtual ~ccGlFilter()
	{
	}

	/**
	 * @brief Get filter description
	 * @return Description string
	 */
	inline virtual QString getDescription() const
	{
		return m_description;
	}

	/**
	 * @brief Clone the filter
	 * @return New filter instance
	 */
	virtual ccGlFilter* clone() const = 0;

	/**
	 * @brief Initialize the filter
	 * @param[in] width Viewport width
	 * @param[in] height Viewport height
	 * @param[in] shadersPath Path to shader files
	 * @param[out] error Error message if failed
	 * @param[in] silent Suppress dialogs
	 * @return true on success
	 */
	virtual bool init(unsigned       width,
	                  unsigned       height,
	                  const QString& shadersPath,
	                  QString&       error,
	                  bool           silent) = 0;

	//! Minimal set of 3D viewport parameters that can be used by shaders
	struct CCFBO_LIB_API ViewportParameters
	{
		//! Default constructor
		ViewportParameters()
		    : perspectiveMode(false)
		    , zNear(0.0)
		    , zFar(1.0)
		    , zoomFactor(1.0)
		{
		}

		//! Whether perspective mode is enabled or not
		bool perspectiveMode;
		//! Near clipping plane position
		double zNear;
		//! Far clipping plane position
		double zFar;
		//! Rendering zoom factor
		float zoomFactor;
	};

	//! Applies filter to texture (depth + color)
	virtual void shade(unsigned            texDepth,
	                   unsigned            texColor,
	                   ViewportParameters& parameters) = 0;

	//! Returns resulting texture
	virtual unsigned getTexture() = 0;

  protected: // methods
	//! Sets whether the filter is valid
	inline void setValid(bool state)
	{
		m_isValid = state;
	}

	//! Returns whether the filter is valid
	inline bool isValid() const
	{
		return m_isValid;
	}

  protected:
	//! Filter validity
	bool m_isValid;

	//! Filter description
	QString m_description;
};
