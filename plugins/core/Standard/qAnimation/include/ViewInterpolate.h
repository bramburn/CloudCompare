#pragma once

//##########################################################################
//#                                                                        #
//#                   CLOUDCOMPARE PLUGIN: qAnimation                      #
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
//#             COPYRIGHT: Ryan Wicks, 2G Robotics Inc., 2015              #
//#                                                                        #
//##########################################################################

/**
 * @file ViewInterpolate.h
 *
 * @brief View interpolation for animations
 *
 * Interpolate between two viewport positions for smooth animations.
 */

#include "ExtendedViewport.h"

class ccPolyline;

/**
 * @class ViewInterpolate
 *
 * @brief View interpolator
 *
 * Interpolate between two viewports over a set number of steps.
 */
class ViewInterpolate
{
public:

	/**
	 * @brief Create interpolator
	 * @param[in] view1 First viewport
	 * @param[in] view2 Second viewport
	 * @param[in] stepCount Number of interpolation steps
	 */
    ViewInterpolate( const ExtendedViewportParameters& view1,  const ExtendedViewportParameters& view2, unsigned int stepCount = 0 );

	/**
	 * @brief Set smooth trajectory
	 * @param[in] smoothTrajectory Smoothed trajectory
	 * @param[in] smoothTrajectoryReversed Reversed trajectory
	 * @param[in] i1 Start index
	 * @param[in] i2 Stop index
	 * @param[in] length Segment length
	 */
	void setSmoothTrajectory(	ccPolyline* smoothTrajectory,
								ccPolyline* smoothTrajectoryReversed,
								unsigned i1,
								unsigned i2,
								PointCoordinateType length);

	/// Get first viewport
	inline const ccViewportParameters& view1 () const { return m_view1.params; }
	/// Get second viewport
	inline const ccViewportParameters& view2 () const { return m_view2.params; }

	/**
	 * @brief Interpolate at ratio
	 * @param[out] viewport Interpolated viewport
	 * @param[in] ratio Interpolation ratio (0-1)
	 * @return Success
	 */
	bool interpolate(ExtendedViewportParameters& viewport, double ratio ) const;
	
	/**
	 * @brief Get next viewport
	 * @param[out] viewport Next viewport
	 * @return Success
	 */
    bool nextView (ExtendedViewportParameters& viewport );

	/// Get current step
	inline unsigned int currentStep () { return m_currentStep; }
	/// Set current step
	inline void setCurrentStep ( unsigned int step ) { m_currentStep = step; }

	/// Get max steps
	inline unsigned int maxStep() { return m_totalSteps; }
	/// Set max steps
	inline void setMaxStep ( unsigned int stepCount ) { m_totalSteps = stepCount; }

	/// Reset interpolator
	inline void reset() { m_currentStep = 0; }

private:

	ExtendedViewportParameters m_view1;
	ExtendedViewportParameters m_view2;

    unsigned int m_totalSteps;
    unsigned int m_currentStep;

	ccPolyline *smoothTrajectory, *smoothTrajectoryReversed;
	unsigned smoothTrajStartIndex, smoothTrajStopIndex, smoothTrajCurrentIndex;
	PointCoordinateType smoothSegmentLength, smoothCurrentLength;

};
