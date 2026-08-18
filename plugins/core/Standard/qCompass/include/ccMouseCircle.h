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
 * @file ccMouseCircle.h
 *
 * @brief Mouse circle overlay
 *
 * Visual circle around mouse cursor for compass measurements.
 */

#include <ccStdPluginInterface.h>
#include <ccGLWindowInterface.h>
#include <cc2DViewportObject.h>

#include <QEvent>
#include <QPoint>
#include <QObject>

/**
 * @class ccMouseCircle
 *
 * @brief Mouse circle overlay
 *
 * Transparent overlay that draws a circle around the mouse cursor.
 */
class ccMouseCircle : public cc2DViewportObject, public QObject
{
public:
	/**
	 * @brief Create mouse circle
	 * @param[in] owner Owner window
	 * @param[in] name Circle name
	 */
	explicit ccMouseCircle(ccGLWindowInterface* owner, QString name = QString("MouseCircle"));

	/// Destructor
	~ccMouseCircle() override;

	/// Get circle radius in pixels
	inline int getRadiusPx() const { return m_radius; }

	/// Get circle radius in world coordinates
	float getRadiusWorld();

	/// Mark owner as dead
	inline void ownerIsDead() { m_owner = nullptr; }

protected:
	/// Draw circle
	void draw(CC_DRAW_CONTEXT& context) override;

private:
	/// Event filter for mouse updates
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	ccGLWindowInterface* m_owner;
	float m_pixelSize;
	int m_radius;
	int m_radiusStep;
};
