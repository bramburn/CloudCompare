// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        //
// ##########################################################################

/**
 * @file ccCircle.h
 *
 * @brief Circle primitive as a polyline.
 *
 * @details Represents a circle as a polyline for visualization
 * and fitting operations.
 *
 * ## Overview
 *
 * A circle is represented as a polyline (series of line segments)
 * for rendering in 3D views.
 *
 * Parameters:
 * - **Radius**: Circle size
 * - **Resolution**: Number of line segments (default 48)
 *
 * Uses:
 * - Fitting to point clouds
 * - Primitive creation
 * - Reference geometry
 * - Cross-section visualization
 *
 * ## Resolution vs. Radius
 *
 * Higher resolution = smoother circle but more vertices.
 * A resolution of 48 gives ~7.5° per segment.
 *
 * ## Usage
 *
 * @code
 * // Create a circle
 * ccCircle* circle = new ccCircle(1.0); // radius = 1.0
 *
 * // Set properties
 * circle->setRadius(2.5);
 * circle->setResolution(64);
 *
 * // Transform
 * ccGLMatrix trans;
 * trans.setTranslation(CCVector3(10, 0, 0));
 * circle->applyGLTransformation(trans);
 *
 * // Clone
 * ccCircle* copy = circle->clone();
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPolyline for base class
 */

#pragma once

// Local
#include "ccPolyline.h"

/**
 * @brief Circle primitive.
 *
 * @details A circle represented as a polyline for visualization.
 *
 * Features:
 * - Configurable radius
 * - Adjustable resolution
 * - Transformation support
 * - Fitting capability
 *
 * @extends ccPolyline
 */
class QCC_DB_LIB_API ccCircle : public ccPolyline
{
  public:
	/**
	 * @brief Create a circle.
	 *
	 * @param[in] radius Circle radius.
	 * @param[in] resolution Number of segments.
	 * @param[in] uniqueID Unique ID.
	 */
	explicit ccCircle(double   radius     = 0.0,
	                  unsigned resolution = 48,
	                  unsigned uniqueID   = ccUniqueIDGenerator::InvalidUniqueID);

	/**
	 * @brief Copy constructor.
	 *
	 * @param[in] circle Circle to copy.
	 */
	ccCircle(const ccCircle& circle);

	/**
	 * @brief Destructor.
	 */
	~ccCircle() override = default;

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::CIRCLE;
	}

	/**
	 * @brief Apply OpenGL transformation.
	 */
	void applyGLTransformation(const ccGLMatrix& trans) override;

	/**
	 * @brief Clone the circle.
	 */
	ccCircle* clone() const;

	/**
	 * @brief Set circle radius.
	 *
	 * @param[in] radius New radius.
	 */
	void setRadius(double radius);

	/**
	 * @brief Get circle radius.
	 */
	inline double getRadius() const
	{
		return m_radius;
	}

	/**
	 * @brief Set display resolution.
	 *
	 * @param[in] resolution Number of segments (>= 4).
	 */
	void setResolution(unsigned resolution);

	/**
	 * @brief Get display resolution.
	 */
	inline unsigned getResolution() const
	{
		return m_resolution;
	}

  protected:
	/**
	 * @brief Update internal representation.
	 */
	void updateInternalRepresentation();

	// Serialization
	/**
	 * @brief Serialize to file.
	 */
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;

	/**
	 * @brief Deserialize from file.
	 */
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;

	/**
	 * @brief Minimum file version.
	 */
	short minimumFileVersion_MeOnly() const override;

  private:
	//! Circle radius.
	double m_radius;

	//! Display resolution (segments).
	unsigned m_resolution;
};
