#pragma once

//##########################################################################
//#                                                                        #
//#                      CLOUDCOMPARE PLUGIN: qSRA                         #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                           COPYRIGHT: EDF                               #
//#                                                                        #
//##########################################################################

/**
 * @file ccMapWindow.h
 *
 * @brief Map window
 *
 * 2D map display window for SRA.
 */

#include <ccGLWindow.h>

#include <ccScalarField.h>

/**
 * @class ccMapWindow
 *
 * @brief 2D map display window
 *
 * 2D map display window with scalar field color ramp.
 */
class ccMapWindow : public ccGLWindow
{
public:

	/**
	 * @brief Create window
	 * @param[in] parent Parent widget
	 */
	explicit ccMapWindow(QOpenGLWidget* parent = nullptr)
		: ccGLWindow(nullptr, parent, true)
		, m_sfForRampDisplay(nullptr)
		, m_showSF(true)
	{}

	/// Destructor
	virtual ~ccMapWindow()
	{
		setAssociatedScalarField(nullptr);
	}

	/**
	 * @brief Set associated scalar field
	 * @param[in] sf Scalar field for color ramp display
	 */
	void setAssociatedScalarField(ccScalarField* sf)
	{
		if (m_sfForRampDisplay != sf)
		{
			if (m_sfForRampDisplay)
				m_sfForRampDisplay->release();
			
			m_sfForRampDisplay = sf;
			
			if (m_sfForRampDisplay)
				m_sfForRampDisplay->link();
		}
	}

	/**
	 * @brief Show/hide scalar field
	 * @param[in] state Show state
	 */
	void showSF(bool state) { m_showSF = state; }

	/// Check if SF is shown
	bool sfShown() const { return m_showSF; }

	/// Get associated scalar field
	ccScalarField* getAssociatedScalarField() const { return m_sfForRampDisplay; }

	/// Get OpenGL context
	void getContext(CC_DRAW_CONTEXT& context) override
	{
		ccGLWindow::getContext(context);

		if (m_showSF)
		{
			context.sfColorScaleToDisplay = m_sfForRampDisplay;
		}
	}
