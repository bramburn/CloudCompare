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

#ifndef CC_FACET_HEADER
#define CC_FACET_HEADER

/**
 * @file ccFacet.h
 *
 * @brief Facet class for planar surface extraction
 *
 * A facet is a composite object representing a planar surface:
 * - Point cloud (origin points)
 * - 2.5D contour polyline
 * - 2.5D surface mesh
 *
 * Facets are created from point clouds and represent flat surfaces
 * like ground planes, walls, or other planar structures.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// Local
#include "ccHObject.h"
#include "ccPlanarEntityInterface.h"

namespace CCCoreLib
{
	class GenericIndexedCloudPersist;
}

class ccMesh;
class ccPolyline;
class ccPointCloud;

/**
 * @brief Facet (planar surface)
 *
 * Composite object: point cloud + 2.5D contour + 2.5D surface mesh.
 * Represents a planar surface extracted from point cloud data.
 */
class QCC_DB_LIB_API ccFacet : public ccHObject
    , public ccPlanarEntityInterface
{
  public:
	/**
	 * @brief Create a facet
	 * @param[in] maxEdgeLength Max edge length (0 = no limit)
	 * @param[in] name Facet name
	 */
	ccFacet(PointCoordinateType maxEdgeLength = 0,
	        const QString&      name          = QString("Facet"));

	/**
	 * @brief Destructor
	 */
	~ccFacet() override = default;

	/**
	 * @brief Create a facet from points
	 *
	 * Creates a planar facet from a point cloud.
	 * The boundary can be convex hull (maxEdgeLength=0) or concave hull.
	 *
	 * @param[in] cloud Source point cloud
	 * @param[in] maxEdgeLength Max edge length (0 = convex hull)
	 * @param[in] transferOwnership Keep source cloud as origin points
	 * @param[in] planeEquation Custom plane equation (optional)
	 * @return New facet, or nullptr on failure
	 */
	static ccFacet* Create(CCCoreLib::GenericIndexedCloudPersist* cloud,
	                       PointCoordinateType                    maxEdgeLength     = 0,
	                       bool                                   transferOwnership = false,
	                       const PointCoordinateType*             planeEquation     = nullptr);

	/**
	 * @brief Get class type
	 * @return CC_TYPES::FACET
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::FACET;
	}
	/**
	 * @brief Check if serializable
	 * @return true
	 */
	bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Set facet color
	 * @param[in] rgb RGB color
	 */
	void setColor(const ccColor::Rgb& rgb);

	// inherited from ccPlanarEntityInterface
	inline CCVector3 getNormal() const override
	{
		return CCVector3(m_planeEquation);
	}

	//! Returns associated RMS
	inline double getRMS() const
	{
		return m_rms;
	}
	//! Returns associated surface
	inline double getSurface() const
	{
		return m_surface;
	}
	//! Returns plane equation
	inline const PointCoordinateType* getPlaneEquation() const
	{
		return m_planeEquation;
	}
	//! Inverts the facet normal
	void invertNormal();
	//! Returns the facet center
	inline const CCVector3& getCenter() const
	{
		return m_center;
	}

	//! Returns polygon mesh (if any)
	inline ccMesh* getPolygon()
	{
		return m_polygonMesh;
	}
	//! Returns polygon mesh (if any)
	inline const ccMesh* getPolygon() const
	{
		return m_polygonMesh;
	}

	//! Returns contour polyline (if any)
	inline ccPolyline* getContour()
	{
		return m_contourPolyline;
	}
	//! Returns contour polyline (if any)
	inline const ccPolyline* getContour() const
	{
		return m_contourPolyline;
	}

	//! Returns contour vertices (if any)
	inline ccPointCloud* getContourVertices()
	{
		return m_contourVertices;
	}
	//! Returns contour vertices (if any)
	inline const ccPointCloud* getContourVertices() const
	{
		return m_contourVertices;
	}

	//! Returns origin points (if any)
	inline ccPointCloud* getOriginPoints()
	{
		return m_originPoints;
	}
	//! Returns origin points (if any)
	inline const ccPointCloud* getOriginPoints() const
	{
		return m_originPoints;
	}

	//! Sets polygon mesh
	inline void setPolygon(ccMesh* mesh)
	{
		m_polygonMesh = mesh;
	}
	//! Sets contour polyline
	inline void setContour(ccPolyline* poly)
	{
		m_contourPolyline = poly;
	}
	//! Sets contour vertices
	inline void setContourVertices(ccPointCloud* cloud)
	{
		m_contourVertices = cloud;
	}
	//! Sets origin points
	inline void setOriginPoints(ccPointCloud* cloud)
	{
		m_originPoints = cloud;
	}

	//! Clones this facet
	ccFacet* clone() const;

  protected:
	// inherited from ccDrawable
	void drawMeOnly(CC_DRAW_CONTEXT& context) override;

	//! Creates internal representation (polygon, polyline, etc.)
	bool createInternalRepresentation(CCCoreLib::GenericIndexedCloudPersist* points,
	                                  const PointCoordinateType*             planeEquation = nullptr);

	//! Facet
	ccMesh* m_polygonMesh;
	//! Facet contour
	ccPolyline* m_contourPolyline;
	//! Shared vertices (between polygon and contour)
	ccPointCloud* m_contourVertices;
	//! Origin points
	ccPointCloud* m_originPoints;

	//! Plane equation - as usual in CC plane equation is ax + by + cz = d
	PointCoordinateType m_planeEquation[4];

	//! Facet centroid
	CCVector3 m_center;

	//! RMS (relatively to m_associatedPoints)
	double m_rms;

	//! Surface (m_polygon)
	double m_surface;

	//! Max length
	PointCoordinateType m_maxEdgeLength;

	// inherited from ccHObject
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	// ccHObject interface
	void applyGLTransformation(const ccGLMatrix& trans) override;
};

#endif // CC_FACET_PRIMITIVE_HEADER
