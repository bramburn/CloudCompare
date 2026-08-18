#pragma once

//##########################################################################
//#                                                                        #
//#                    CLOUDCOMPARE PLUGIN: ccCompass                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                     COPYRIGHT: Sam Thiele  2017                        #
//#                                                                        #
//##########################################################################

/**
 * @file ccMeasurement.h
 *
 * @brief Measurement base class
 *
 * Base class for compass measurements.
 */

#include <ccHObject.h>
#include <ccPointCloud.h>
#include <ccMainAppInterface.h>

/**
 * @class ccMeasurement
 *
 * @brief Measurement base class
 *
 * Base class for geological measurements with drawing states.
 */
class ccMeasurement
{
public:
	/// Constructor
	ccMeasurement() {}

	/// Destructor
	virtual ~ccMeasurement() {}

	/// Set default color
	void setDefaultColor  (const ccColor::Rgb& col) { m_normal_colour    = col; }
	
	/// Set highlight color
	void setHighlightColor(const ccColor::Rgb& col) { m_highlight_colour = col; }
	
	/// Set active color
	void setActiveColor   (const ccColor::Rgb& col) { m_active_colour    = col; }
	
	/// Set alternate color
	void setAlternateColor(const ccColor::Rgb& col) { m_alternate_colour = col; }

	/// Get measurement color based on state
	ccColor::Rgb getMeasurementColour() const
	{
		if (m_isActive)
		{
			return m_active_colour;
		}
		else if (m_isAlternate)
		{
			return m_alternate_colour;
		}
		else if (m_isHighlighted)
		{
			return m_highlight_colour;
		}
		return m_normal_colour;
	}

	/// Set active state
	void setActive   (bool isActive) { m_isActive      = isActive; }
	/// Set highlight state
	void setHighlight(bool isActive) { m_isHighlighted = isActive; }
	/// Set alternate state
	void setAlternate(bool isActive) { m_isAlternate   = isActive;  }
	/// Set normal state
	void setNormal() { m_isActive = false; m_isHighlighted = false; m_isAlternate = false; }

protected:
	bool m_isActive = false;
	bool m_isHighlighted = false;
	bool m_isAlternate = false;
	ccColor::Rgb m_active_colour = ccColor::yellow;
	ccColor::Rgb m_highlight_colour = ccColor::green;
	ccColor::Rgb m_alternate_colour = ccColor::cyan;
	ccColor::Rgb m_normal_colour = ccColor::blue;
};
