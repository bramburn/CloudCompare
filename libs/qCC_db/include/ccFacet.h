// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccFacet.h
 *
 * @brief Facet class for planar surface extraction.
 *
 * @details A facet is a composite object representing a planar surface:
 * - Point cloud (origin points)
 * - 2.5D contour polyline
 * - 2.5D surface mesh
 *
 * Facets are created from point clouds and represent flat surfaces
 * like ground planes, walls, or other planar structures.
 *
 * ## Facet Structure
 *
 * A facet consists of:
 * - **Polygon mesh**: Triangulated surface
 * - **Contour polyline**: Boundary of the facet
 * - **Contour vertices**: Shared vertices
 * - **Origin points**: Original points belonging to the facet
 *
 * ## Properties
 *
 * - **Plane equation**: ax + by + cz = d
 * - **Normal vector**: (a, b, c)
 * - **Centroid**: Center point of the facet
 * - **RMS**: Root mean square deviation
 * - **Surface**: Facet area
 *
 * ## Usage
 *
 * @code
 * // Create facet from point cloud
 * ccFacet* facet = ccFacet::Create(cloud, maxEdgeLength);
 *
 * // Get facet properties
 * CCVector3 normal = facet->getNormal();
 * double surface = facet->getSurface();
 * double rms = facet->getRMS();
 *
 * // Get mesh and contour
 * ccMesh* mesh = facet->getPolygon();
 * ccPolyline* contour = facet->getContour();
 *
 * // Clone facet
 * ccFacet* cloned = facet->clone();
 * @endcode
 *
 * @extends ccHObject
 * @extends ccPlanarEntityInterface
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccPlanarEntityInterface for planar entity interface
 */

#ifndef CC_FACET_HEADER
#define CC_FACET_HEADER

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
 * @brief Facet (planar surface).
 *
 * @details Composite object representing a planar surface.
 * Contains point cloud, 2.5D contour, and 2.5D surface mesh.
 *
 * @extends ccHObject
 * @extends ccPlanarEntityInterface
 */
class QCC_DB_LIB_API ccFacet : public ccHObject
    , public ccPlanarEntityInterface
{
  public:
	/**
	 * @brief Create a facet.
	 *
	 * @param[in] maxEdgeLength Max edge length (0 = no limit).
	 * @param[in] name Facet name.
	 */
	ccFacet(PointCoordinateType maxEdgeLength = 0,
	        const QString&      name          = QString("Facet"));

	/**
	 * @brief Destructor.
	 */
	~ccFacet() override = default;

	/**
	 * @brief Create a facet from points.
	 *
	 * Creates a planar facet from a point cloud.
	 * The boundary can be convex hull (maxEdgeLength=0) or concave hull.
	 *
	 * @param[in] cloud Source point cloud.
	 * @param[in] maxEdgeLength Max edge length (0 = convex hull).
	 * @param[in] transferOwnership Keep source cloud as origin points.
	 * @param[in] planeEquation Custom plane equation (optional).
	 *
	 * @return New facet, or nullptr on failure.
	 */
	static ccFacet* Create(CCCoreLib::GenericIndexedCloudPersist* cloud,
	                       PointCoordinateType                    maxEdgeLength     = 0,
	                       bool                                   transferOwnership = false,
	                       const PointCoordinateType*             planeEquation     = nullptr);

	/**
	 * @brief Get class type.
	 */
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::FACET;
	}

	/**
	 * @brief Check if serializable.
	 */
	bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Set facet color.
	 *
	 * @param[in] rgb RGB color.
	 */
	void setColor(const ccColor::Rgb& rgb);

	// inherited from ccPlanarEntityInterface
	/**
	 * @brief Get facet normal.
	 */
	inline CCVector3 getNormal() const override
	{
		return CCVector3(m_planeEquation);
	}

	/**
	 * @brief Get RMS (root mean square).
	 */
	inline double getRMS() const
	{
		return m_rms;
	}

	/**
	 * @brief Get surface area.
	 */
	inline double getSurface() const
	{
		return m_surface;
	}

	/**
	 * @brief Get plane equation.
	 *
	 * @return Plane equation (a, b, c, d) where ax + by + cz = d.
	 */
	inline const PointCoordinateType* getPlaneEquation() const
	{
		return m_planeEquation;
	}

	/**
	 * @brief Invert the facet normal.
	 */
	void invertNormal();

	/**
	 * @brief Get facet centroid.
	 */
	inline const CCVector3& getCenter() const
	{
		return m_center;
	}

	/**
	 * @brief Get polygon mesh.
	 */
	inline ccMesh* getPolygon()
	{
		return m_polygonMesh;
	}

	/**
	 * @brief Get polygon mesh (const).
	 */
	inline const ccMesh* getPolygon() const
	{
		return m_polygonMesh;
	}

	/**
	 * @brief Get contour polyline.
	 */
	inline ccPolyline* getContour()
	{
		return m_contourPolyline;
	}

	/**
	 * @brief Get contour polyline (const).
	 */
	inline const ccPolyline* getContour() const
	{
		return m_contourPolyline;
	}

	/**
	 * @brief Get contour vertices.
	 */
	inline ccPointCloud* getContourVertices()
	{
		return m_contourVertices;
	}

	/**
	 * @brief Get contour vertices (const).
	 */
	inline const ccPointCloud* getContourVertices() const
	{
		return m_contourVertices;
	}

	/**
	 * @brief Get origin points.
	 */
	inline ccPointCloud* getOriginPoints()
	{
		return m_originPoints;
	}

	/**
	 * @brief Get origin points (const).
	 */
	inline const ccPointCloud* getOriginPoints() const
	{
		return m_originPoints;
	}

	/**
	 * @brief Set polygon mesh.
	 *
	 * @param[in] mesh Polygon mesh.
	 */
	inline void setPolygon(ccMesh* mesh)
	{
		m_polygonMesh = mesh;
	}

	/**
	 * @brief Set contour polyline.
	 *
	 * @param[in] poly Contour polyline.
	 */
	inline void setContour(ccPolyline* poly)
	{
		m_contourPolyline = poly;
	}

	/**
	 * @brief Set contour vertices.
	 *
	 * @param[in] cloud Contour vertices.
	 */
	inline void setContourVertices(ccPointCloud* cloud)
	{
		m_contourVertices = cloud;
	}

	/**
	 * @brief Set origin points.
	 *
	 * @param[in] cloud Origin points.
	 */
	inline void setOriginPoints(ccPointCloud* cloud)
	{
		m_originPoints = cloud;
	}

	/**
	 * @brief Clone this facet.
	 */
	ccFacet* clone() const;

  protected:
	/**
	 * @brief Draw the facet.
	 */
	void drawMeOnly(CC_DRAW_CONTEXT& context) override;

	/**
	 * @brief Create internal representation.
	 *
	 * @param[in] points Points to use.
	 * @param[in] planeEquation Optional plane equation.
	 *
	 * @return true if successful.
	 */
	bool createInternalRepresentation(CCCoreLib::GenericIndexedCloudPersist* points,
	                                  const PointCoordinateType*             planeEquation = nullptr);

  private:
	//! Polygon mesh.
	ccMesh* m_polygonMesh;

	//! Contour polyline.
	ccPolyline* m_contourPolyline;

	//! Shared vertices.
	ccPointCloud* m_contourVertices;

	//! Origin points.
	ccPointCloud* m_originPoints;

	//! Plane equation (ax + by + cz = d).
	PointCoordinateType m_planeEquation[4];

	//! Facet centroid.
	CCVector3 m_center;

	//! RMS deviation.
	double m_rms;

	//! Surface area.
	double m_surface;

	//! Max edge length.
	PointCoordinateType m_maxEdgeLength;

	// Serialization
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	// GL transformation
	void applyGLTransformation(const ccGLMatrix& trans) override;
};

#endif // CC_FACET_PRIMITIVE_HEADER
