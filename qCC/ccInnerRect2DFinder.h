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

#ifndef CC_INNER_RECT_2D_FINDER_HEADER
#define CC_INNER_RECT_2D_FINDER_HEADER

/**
 * @file ccInnerRect2DFinder.h
 *
 * @brief Inner rectangle finder algorithm.
 *
 * @details Finds the largest inscribed (inner) rectangle within a
 * 2D point cloud.
 *
 * Given a set of 2D points forming a boundary, this algorithm
 * finds the maximum-area axis-aligned rectangle that is completely
 * contained within the point cloud.
 *
 * This is useful for:
 * - Finding usable area in scanned surfaces
 * - Determining valid regions for processing
 * - Creating rectangular bounds within irregular shapes
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccBox
 */

#include <ccBox.h>
#include <ccGenericPointCloud.h>

/**
 * @brief Algorithm for finding the largest inscribed rectangle.
 *
 * @details Finds the maximum-area axis-aligned rectangle that
 * fits inside a 2D point cloud boundary.
 *
 * The algorithm works by:
 * 1. Computing the bounding rectangle
 * 2. Iteratively testing and shrinking edges
 * 3. Finding the largest rectangle that fits
 *
 * Returns a 3D box primitive with the rectangle as its base.
 */
class ccInnerRect2DFinder
{

  public:
	/**
	 * @brief Construct the finder.
	 */
	ccInnerRect2DFinder();

	/**
	 * @brief Find the biggest inscribed rectangle.
	 *
	 * @param[in] cloud Point cloud (2D projected).
	 * @param[in] zDim Dimension index for extrusion (0=X, 1=Y, 2=Z).
	 *
	 * @return Box primitive with the rectangle as base.
	 *
	 * @note The returned box must be deleted by caller.
	 */
	ccBox* process(ccGenericPointCloud* cloud, unsigned char zDim = 2);

  protected:
	/**
	 * @brief Initialize internal structures.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] zDim Z dimension index.
	 * @return true on success.
	 */
	bool init(ccGenericPointCloud* cloud, unsigned char zDim);

	/**
	 * @brief 2D rectangle structure.
	 */
	struct Rect
	{
		Rect()
		    : x0(0)
		    , y0(0)
		    , x1(0)
		    , y1(0)
		{
		}

		Rect(double _x0, double _y0, double _x1, double _y1)
		    : x0(_x0)
		    , y0(_y0)
		    , x1(_x1)
		    , y1(_y1)
		{
		}

		//! Coordinates
		double x0, y0, x1, y1;

		/**
		 * @brief Get rectangle width.
		 */
		inline double width() const
		{
			return x1 - x0;
		}

		/**
		 * @brief Get rectangle height.
		 */
		inline double height() const
		{
			return y1 - y0;
		}

		/**
		 * @brief Get rectangle area.
		 */
		inline double area() const
		{
			return width() * height();
		}
	};

	/**
	 * @brief Recursive rectangle finding.
	 *
	 * @param[in] rect Current rectangle.
	 * @param[in] startIndex Starting point index.
	 */
	void findBiggestRect(const Rect& rect, unsigned startIndex);

	//! Bounding rectangle
	Rect m_boundingRect;

	//! Maximum inscribed rectangle
	Rect m_maxRect;

	//! Maximum area
	double m_maxArea;

	//! Point cloud
	ccGenericPointCloud* m_cloud;

	//! X dimension index
	unsigned char m_X;

	//! Y dimension index
	unsigned char m_Y;
};

#endif // CC_INNER_RECT_2D_FINDER_HEADER
