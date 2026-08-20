#ifndef ISOLINES_HEADER
#define ISOLINES_HEADER

/**
 * @file ccIsolines.h
 *
 * @brief Marching squares isolines implementation.
 *
 * @details Fast implementation of the marching squares algorithm for extracting
 * contour lines (isovalues) from 2D scalar fields. This is a template class
 * that works with any scalar data type.
 *
 * The algorithm traces contour lines through a 2D grid by examining each
 * cell's configuration (which corners are above/below the threshold).
 * Contours can be open (ending at grid boundaries) or closed (loops).
 *
 * @note This is a transcription of FindIsolines.java by Murphy Stein and
 * Greg Borenstein (New York University), originally developed in 2012.
 *
 * @author Murphy Stein, Greg Borenstein (New York University)
 * @author Daniel Girardeau-Montaut (EDF R&D) - modifications
 *
 * @date 2012-2014
 *
 * @see Wikipedia: https://en.wikipedia.org/wiki/Marching_squares
 */

/**
 * Transcription of FindIsolines.java for C++
 *
 * Fast implementation of marching squares
 *
 * @author Murphy Stein, Greg Borenstein
 *        New York University
 * @created Jan-Sept 2012
 * @modified Dec 2014 (DGM)
 *
 * @license BSD
 *
 * Copyright (c) 2012 New York University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by New York University. The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

// qCC_db
#include <ccLog.h>

// system
#include <assert.h>
#include <cmath>
#include <vector>

/**
 * @brief Marching squares isolines for 2D scalar fields.
 *
 * @details A template class implementing the marching squares algorithm
 * for extracting contour lines (isovalues) from 2D scalar fields.
 *
 * Key features:
 * - Supports any scalar data type (float, double, int, etc.)
 * - Extracts both closed contours (loops) and open contours (ending at boundaries)
 * - Computes geometric properties (area, perimeter, curvature)
 * - Point-in-polygon and contour containment tests
 *
 * Usage example:
 * @code
 * // Create isoline extractor for a 100x100 grid
 * Isolines<double> isolines(100, 100);
 *
 * // Set the threshold value for contour extraction
 * isolines.setThreshold(0.5);
 *
 * // Find all contours at the threshold
 * int numContours = isolines.find(gridData);
 *
 * // Iterate through contours
 * for (int i = 0; i < numContours; i++) {
 *     int length = isolines.getContourLength(i);
 *     bool closed = isolines.isContourClosed(i);
 *     double area = isolines.measureArea(i);
 *
 *     // Get contour points
 *     for (int j = 0; j < length; j++) {
 *         double x, y;
 *         isolines.getContourPoint(i, j, x, y);
 *         // Process point...
 *     }
 * }
 * @endcode
 *
 * @note The marching squares algorithm produces ambiguous results for
 * saddle points (cases 5 and 10). This implementation uses the
 * average value method for disambiguation.
 *
 * @tparam T Scalar data type (e.g., float, double, int)
 */
template <typename T>
class Isolines
{
  protected:
	//! Bounding box minima (x, y) for each contour
	std::vector<double> m_minx;
	std::vector<double> m_miny;

	//! Bounding box maxima (x, y) for each contour
	std::vector<double> m_maxx;
	std::vector<double> m_maxy;

	//! Cell codes (4-bit values indicating corner states)
	std::vector<int> m_cd;

	//! X coordinates of all contour vertices (concatenated)
	std::vector<double> m_contourX;

	//! Y coordinates of all contour vertices (concatenated)
	std::vector<double> m_contourY;

	//! Length of each contour
	std::vector<int> m_contourLength;

	//! Starting index of each contour in the vertex arrays
	std::vector<int> m_contourOrigin;

	//! Grid cell index for each vertex
	std::vector<int> m_contourIndexes;

	//! Whether each contour is closed (loop) or open
	std::vector<bool> m_contourClosed;

	//! Grid dimensions
	int m_w;  //!< Grid width
	int m_h;  //!< Grid height

	//! Number of contours found
	int m_numContours;

	//! Threshold value for contour extraction
	T m_threshold;

  public:
	/**
	 * @brief Construct an isoline extractor for a grid.
	 *
	 * @param[in] w Grid width.
	 * @param[in] h Grid height.
	 *
	 * @details Allocates internal buffers for the given grid size.
	 * Negative dimensions are clamped to zero.
	 */
	Isolines(int w, int h)
	    : m_w(std::max(0, w))
	    , m_h(std::max(0, h))
	    , m_numContours(0)
	    , m_threshold(0)
	{
		// Allocate cell code array
		m_cd.resize(static_cast<size_t>(w) * static_cast<size_t>(h), 0);
	}

	/**
	 * @brief Set the threshold value for contour extraction.
	 *
	 * @param[in] t Threshold value.
	 *
	 * @details Points with values >= threshold form the "above" region,
	 * points with values < threshold form the "below" region.
	 */
	inline void setThreshold(T t)
	{
		m_threshold = t;
	}

	/**
	 * @brief Find all contours at the current threshold.
	 *
	 * @param[in] in Pointer to the 2D scalar field data (row-major).
	 * @return Number of contours found, or -1 on memory error.
	 *
	 * @details Scans the grid to extract all contour lines at the
	 * configured threshold value. Results can be queried using the
	 * getNumContours(), getContourLength(), etc. methods.
	 */
	int find(const T* in)
	{
		preCodeImage(in);
		return findIsolines(in);
	}

	/**
	 * @brief Get the number of found contours.
	 *
	 * @return Number of contours.
	 */
	inline int getNumContours() const
	{
		return m_numContours;
	}

	/**
	 * @brief Get the number of vertices in a contour.
	 *
	 * @param[in] contour Contour index.
	 * @return Number of vertices.
	 */
	inline int getContourLength(int contour) const
	{
		return m_contourLength[contour];
	}

	/**
	 * @brief Check if a contour is closed (loop).
	 *
	 * @param[in] contour Contour index.
	 * @return true if closed, false if open.
	 */
	inline bool isContourClosed(int contour) const
	{
		return m_contourClosed[contour];
	}

	/**
	 * @brief Get a specific point on a contour.
	 *
	 * @param[in] contour Contour index.
	 * @param[in] index Vertex index within the contour.
	 * @param[out] x X coordinate.
	 * @param[out] y Y coordinate.
	 */
	void getContourPoint(int contour, size_t index, double& x, double& y) const
	{
		assert(static_cast<int>(index) < getContourLength(contour));
		x = getContourX(contour, index);
		y = getContourY(contour, index);
	}

	/**
	 * @brief Measure the area enclosed by a contour.
	 *
	 * @param[in] contour Contour index.
	 * @return Area in square units.
	 *
	 * @note Only meaningful for closed contours.
	 */
	inline double measureArea(int contour) const
	{
		return measureArea(contour, 0, getContourLength(contour));
	}

	/**
	 * @brief Measure the perimeter of a contour.
	 *
	 * @param[in] contour Contour index.
	 * @return Perimeter length.
	 */
	inline double measurePerimeter(int contour) const
	{
		return measurePerimeter(contour, 0, getContourLength(contour));
	}

	/**
	 * @brief Create a single-pixel border around the grid.
	 *
	 * @param[in,out] inout Grid data.
	 * @param[in] borderval Value to set on the border.
	 *
	 * @details Sets the outermost row and column to a constant value,
	 * ensuring contours that would otherwise exit the grid are closed.
	 */
	void createOnePixelBorder(T* inout, T borderval) const
	{
		// Top and bottom rows
		{
			int shift = (m_h - 1) * m_w;
			for (int i = 0; i < m_w; i++)
			{
				inout[i] = inout[i + shift] = borderval;
			}
		}
		// Left and right columns
		{
			for (int j = 0; j < m_h; j++)
			{
				inout[j * m_w] = inout[(j + 1) * m_w - 1] = borderval;
			}
		}
	}

  protected:
	/**
	 * @brief Pre-compute cell codes for the grid.
	 *
	 * @param[in] in Grid data.
	 *
	 * @details Each 2x2 cell gets a 4-bit code indicating which corners
	 * are below the threshold. This determines the cell's configuration
	 * for the marching squares algorithm.
	 *
	 * Code bits: bottom-left=1, bottom-right=2, top-right=4, top-left=8
	 */
	void preCodeImage(const T* in)
	{
		for (int x = 0; x < m_w - 1; x++)
		{
			for (int y = 0; y < m_h - 1; y++)
			{
				int b0(in[ixy(x + 0, y + 1)] < m_threshold ? 1 : 0); // bottom-left
				int b1(in[ixy(x + 1, y + 1)] < m_threshold ? 2 : 0); // bottom-right
				int b2(in[ixy(x + 1, y + 0)] < m_threshold ? 4 : 0); // top-right
				int b3(in[ixy(x + 0, y + 0)] < m_threshold ? 8 : 0); // top-left
				m_cd[ixy(x, y)] = b0 | b1 | b2 | b3;
			}
		}
	}

	//! Marching squares 2x2 cell configuration codes
	enum ConfigurationCodes
	{
		CASE0   = 0,  //!< All corners above threshold
		CASE1   = 1,  //!< Only bottom-left below
		CASE2   = 2,  //!< Only bottom-right below
		CASE3   = 3,  //!< Bottom corners below
		CASE4   = 4,  //!< Only top-right below
		CASE5   = 5,  //!< Bottom-left and top-right below (saddle)
		CASE6   = 6,  //!< Right corners below
		CASE7   = 7,  //!< All except top-left below
		CASE8   = 8,  //!< Only top-left below
		CASE9   = 9,  //!< Left corners below
		CASE10  = 10, //!< Bottom-left and top-right above (saddle)
		CASE11  = 11, //!< All except bottom-right below
		CASE12  = 12, //!< Top corners below
		CASE13  = 13, //!< All except bottom-left below
		CASE14  = 14, //!< All except top-left below
		CASE15  = 15, //!< All corners below
		VISITED = 16  //!< Cell has been processed
	};

	//! Entry/exit edge indices for marching
	enum Edges
	{
		NONE   = -1,
		TOP    = 0,
		RIGHT  = 1,
		BOTTOM = 2,
		LEFT   = 3
	};

	//! Finalize a contour and handle path merging
	void endContour(bool closed, bool alternatePath);

	//! Core marching squares algorithm
	int findIsolines(const T* in);

	/**
	 * @brief Linear interpolation for finding contour crossings.
	 *
	 * @param[in] A Value at position 0.
	 * @param[in] B Value at position 1.
	 * @return Interpolation factor where value equals threshold.
	 *
	 * @details Computes (A - threshold) / (A - B) to find where along
	 * an edge the contour crosses.
	 */
	inline double LERP(T A, T B) const
	{
		T AB = A - B;
		return AB == 0 ? 0 : static_cast<double>(A - m_threshold) / AB;
	}

	/**
	 * @brief Get the last vertex index across all contours.
	 */
	inline int getLastIndex() const
	{
		int nc = getNumContours();
		return nc > 0 ? m_contourOrigin[nc - 1] + m_contourLength[nc - 1] : 0;
	}

	/**
	 * @brief Set X coordinate of a contour vertex.
	 */
	inline void setContourX(int contour, int v, double x)
	{
		int o = m_contourOrigin[contour];
		m_contourX[wrap(o + v, o, o + m_contourLength[contour])] = x;
	}

	/**
	 * @brief Set Y coordinate of a contour vertex.
	 */
	inline void setContourY(int contour, int v, double y)
	{
		int o = m_contourOrigin[contour];
		m_contourY[wrap(o + v, o, o + m_contourLength[contour])] = y;
	}

	/**
	 * @brief Get wrapped index for circular array access.
	 */
	inline int getValidIndex(int contour, int v) const
	{
		int o = m_contourOrigin[contour];
		return wrap(o + v, o, o + m_contourLength[contour]);
	}

	/**
	 * @brief Measure area of a contour segment.
	 *
	 * @param[in] contour Contour index.
	 * @param[in] first Start vertex index.
	 * @param[in] last End vertex index.
	 * @return Area.
	 */
	double measureArea(int contour, int first, int last) const;

	/**
	 * @brief Get mean X coordinate of contour vertices.
	 */
	double measureMeanX(int contour) const;

	/**
	 * @brief Get mean Y coordinate of contour vertices.
	 */
	double measureMeanY(int contour) const;

	/**
	 * @brief Measure perimeter of a contour segment.
	 */
	double measurePerimeter(int contour, int first, int last) const;

	/**
	 * @brief Get X component of contour normal at a vertex.
	 */
	double measureNormalX(int contour, int i) const;

	/**
	 * @brief Get Y component of contour normal at a vertex.
	 */
	double measureNormalY(int contour, int i) const;

	/**
	 * @brief Get sum of Y normals for a contour segment.
	 */
	double measureNormalY(int contour, int first, int last) const;

	/**
	 @brief Get sum of X normals for a contour segment.
	 */
	double measureNormalX(int contour, int first, int last) const;

	/**
	 * @brief Sum of angle changes along contour segment.
	 */
	double measureAngleChange(int contour, int first, int last) const;

	/**
	 * @brief Wrap index for circular array access.
	 *
	 * @param[in] i Index to wrap.
	 * @param[in] lo Lower bound (inclusive).
	 * @param[in] hi Upper bound (exclusive).
	 * @return Wrapped index.
	 */
	static int wrap(int i, int lo, int hi)
	{
		int l = hi - lo;
		int d = i - lo;
		int w = 0;
		if (d < 0)
			w = hi - ((-d) % l);
		else
			w = lo + (d % l);

		if (w == hi)
			w = lo;

		if (w < lo)
		{
			assert(false);
			printf("went below lo\n");
		}
		else if (w >= hi)
		{
			assert(false);
			printf("went above hi\n");
		}

		return w;
	}

	/**
	 * @brief Convert 2D grid coordinates to 1D index.
	 */
	inline int ixy(int x, int y) const
	{
		return x + y * m_w;
	}

	/**
	 * @brief Euclidean distance between two contour vertices.
	 */
	inline double measureDistance(int contour, int first, int second) const
	{
		double dx = getContourX(contour, first) - getContourX(contour, second);
		double dy = getContourY(contour, first) - getContourY(contour, second);
		return std::sqrt(dx * dx + dy * dy);
	}

	/**
	 * @brief Edge length between consecutive vertices.
	 */
	double measureLength(int contour, int i) const;

	/**
	 * @brief Angle change at a contour vertex.
	 *
	 * @return Angle in radians (positive = counterclockwise).
	 */
	double measureAngle(int contour, int i) const;

  public:
	/**
	 * @brief Get X coordinate of a contour vertex.
	 */
	inline double getContourX(int contour, int v) const
	{
		int o = m_contourOrigin[contour];
		return m_contourX[wrap(o + v, o, o + m_contourLength[contour])];
	}

	/**
	 * @brief Get Y coordinate of a contour vertex.
	 */
	inline double getContourY(int contour, int v) const
	{
		int o = m_contourOrigin[contour];
		return m_contourY[wrap(o + v, o, o + m_contourLength[contour])];
	}

	/**
	 * @brief Local curvature at a contour vertex.
	 *
	 * @return Angle change divided by edge length.
	 */
	inline double measureCurvature(int contour, int i) const
	{
		return measureAngle(contour, i) / measureLength(contour, i);
	}

	/**
	 * @brief Find areas near each contour vertex.
	 *
	 * @param[in] window Number of neighboring vertices on each side.
	 * @param[out] tips Output array (size = grid width x height).
	 *
	 * @details Computes the area of each contour segment within
	 * a window around each vertex. Useful for finding tips/ends.
	 */
	void findAreas(int window, std::vector<double>& tips);

	/**
	 * @brief Find rounded corners in contours.
	 *
	 * @param[in] window Window size for neighborhood.
	 * @param[out] tips Output array.
	 *
	 * @details Computes area/perimeter ratio for local neighborhoods.
	 * High values indicate rounded corners.
	 */
	void findRoundedCorners(int window, std::vector<double>& tips);

	/**
	 * @brief Get the index of the longest contour.
	 *
	 * @return Contour index with maximum length.
	 */
	int getMaxContour() const;

	/**
	 * @brief Compute bounding boxes for all contours.
	 *
	 * @return true on success, false on memory error.
	 */
	bool computeBoundingBoxes();

	//! Get bounding box minimum X
	inline double getBBMinX(int contour) const { return m_minx[contour]; }

	//! Get bounding box maximum X
	inline double getBBMaxX(int contour) const { return m_maxx[contour]; }

	//! Get bounding box minimum Y
	inline double getBBMinY(int contour) const { return m_miny[contour]; }

	//! Get bounding box maximum Y
	inline double getBBMaxY(int contour) const { return m_maxy[contour]; }

	/**
	 * @brief Point-in-polygon test using ray casting.
	 *
	 * @param[in] k Contour index.
	 * @param[in] x Test point X.
	 * @param[in] y Test point Y.
	 * @return true if point is inside contour.
	 */
	bool contains(int k, double x, double y) const;

	/**
	 * @brief Test if one contour contains another.
	 *
	 * @param[in] k1 Outer contour.
	 * @param[in] k2 Inner contour (to test).
	 * @return true if k1 contains k2.
	 */
	bool containsContour(int k1, int k2);

	/**
	 * @brief Test if contour contains a bounding box.
	 */
	inline bool containsBoundingBox(int k, double minx, double miny, double maxx, double maxy) const
	{
		return (contains(k, minx, miny)
		        && contains(k, maxx, miny)
		        && contains(k, maxx, maxy)
		        && contains(k, minx, maxy));
	}

	/**
	 * @brief Point-in-polygon test for arbitrary polygon.
	 */
	bool contains(const std::vector<double>& polyx, const std::vector<double>& polyy, double x, double y) const;

	/**
	 * @brief Test if bounding boxes intersect.
	 */
	bool bbIntersect(int k1, int k2) const;

	/**
	 * @brief Test if bounding box k1 contains k2.
	 */
	bool bbContainsBB(int k1, int k2) const;

	//! Area of contour's bounding box
	inline double bbArea(int k) const
	{
		double w = getBBMaxX(k) - getBBMinX(k);
		double h = getBBMaxY(k) - getBBMinY(k);
		return w * h;
	}

	//! X coordinate of bounding box center
	inline double getBBCenterX(int k) const
	{
		return (getBBMinX(k) + getBBMaxX(k)) / 2.0;
	}

	//! Y coordinate of bounding box center
	inline double getBBCenterY(int k) const
	{
		return (getBBMinY(k) + getBBMaxY(k)) / 2.0;
	}
};

#endif // ISOLINES_HEADER
