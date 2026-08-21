// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_PLANE_PRIMITIVE_HEADER
#define CC_PLANE_PRIMITIVE_HEADER

/**

 * @file ccPlane.h
 *
 * @brief Plane primitive for ground fitting, clipping, and texturing
 *
 * Represents a finite rectangular 3D plane with a normal direction
 * defined by its transformation matrix. Implements ccPlanarEntityInterface
 * so it can be used as a reference plane for measurements and projections.
 *
 * The plane extends ccGenericPrimitive: it has a quadrilateral mesh
 * (2 triangles), positioned in 3D space by the transformation matrix.
 *
 * Key capabilities:
 * - Arbitrary orientation via transformation matrix
 * - Texture mapping with QImage
 * - Plane fitting to point clouds
 * - Equation query (normal + constant)
 * - Width/height modification
 *
 * Texture mapping: the plane UV coordinates map the texture onto the
 * mesh from corner (0,0) to (width, height). The plane's local Z
 * axis (column 2 of the transformation matrix) is the plane normal.
 *
 * @extends ccGenericPrimitive
 * @extends ccPlanarEntityInterface
 */

#include "ccGenericPrimitive.h"
#include "ccPlanarEntityInterface.h"

/**
 * @class ccPlane
 *
 * @brief Rectangular 3D plane primitive
 *
 * A plane is defined by:
 * - Width (X dimension) and height (Y dimension)
 * - Transformation matrix: position and orientation
 *   (local Z axis = plane normal, local XY = plane tangent)
 * - Optionally: texture/image mapped onto the surface
 *
 * Mesh: 2 triangles, 4 vertices, with texture coordinates.
 *
 * @extends ccGenericPrimitive
 * @extends ccPlanarEntityInterface
 */
class QCC_DB_LIB_API ccPlane : public ccGenericPrimitive
    , public ccPlanarEntityInterface
{
  public:
	/**
	 * @brief Create a plane with specified dimensions
	 *
	 * @param[in] xWidth Width along the plane's X axis
	 * @param[in] yWidth Height along the plane's Y axis
	 * @param[in] transMat Optional transformation (position + orientation)
	 * @param[in] name Display name
	 */
	ccPlane(PointCoordinateType xWidth,
	        PointCoordinateType yWidth,
	        const ccGLMatrix*   transMat = nullptr,
	        QString             name     = QString("Plane"));

	/**
	 * @brief Simplified constructor for ccHObject factory
	 *
	 * @param[in] name Display name
	 */
	explicit ccPlane(QString name = QString("Plane"));

	// ccHObject
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::PLANE;
	}

	// ccGenericPrimitive
	virtual QString getTypeName() const override
	{
		return "Plane";
	}
	virtual ccGenericPrimitive* clone() const override;

	// ccHObject
	virtual ccBBox getOwnFitBB(ccGLMatrix& trans) override;

	/**
	 * @brief Get the plane width (X dimension)
	 */
	PointCoordinateType getXWidth() const
	{
		return m_xWidth;
	}

	/**
	 * @brief Get the plane height (Y dimension)
	 */
	PointCoordinateType getYWidth() const
	{
		return m_yWidth;
	}

	/**
	 * @brief Get the plane center position
	 *
	 * Equal to the translation component of the transformation matrix.
	 */
	CCVector3 getCenter() const
	{
		return m_transformation.getTranslationAsVec3D();
	}

	/**
	 * @brief Set the plane width (X dimension)
	 *
	 * @param[in] w New width
	 * @param[in] autoUpdate Rebuild the mesh if true
	 */
	void setXWidth(PointCoordinateType w, bool autoUpdate = true)
	{
		m_xWidth = w;
		if (autoUpdate)
			updateRepresentation();
	}

	/**
	 * @brief Set the plane height (Y dimension)
	 *
	 * @param[in] h New height
	 * @param[in] autoUpdate Rebuild the mesh if true
	 */
	void setYWidth(PointCoordinateType h, bool autoUpdate = true)
	{
		m_yWidth = h;
		if (autoUpdate)
			updateRepresentation();
	}

	// ccPlanarEntityInterface

	/**
	 * @brief Get the plane normal vector
	 *
	 * Returns the local Z axis of the transformation matrix.
	 *
	 * @return Normal direction (unit vector)
	 */
	CCVector3 getNormal() const override
	{
		return m_transformation.getColumnAsVec3D(2);
	}

	/**
	 * @brief Apply a texture image to the plane
	 *
	 * @param[in] image QImage to map onto the plane
	 * @param[in] imageFilename Optional filename (for metadata)
	 * @return The created material, or nullptr on failure
	 */
	ccMaterial::Shared setAsTexture(QImage image, QString imageFilename = QString());

	/**
	 * @brief Apply a texture to a quad mesh
	 *
	 * @param[in] quadMesh Target mesh (must be a quad or have 4 vertices)
	 * @param[in] image QImage to map
	 * @param[in] imageFilename Optional filename
	 * @return The created material, or nullptr on failure
	 */
	static ccMaterial::Shared SetQuadTexture(ccMesh* quadMesh, QImage image, QString imageFilename = QString());

	/**
	 * @brief Fit a plane to a point cloud using least squares
	 *
	 * Uses principal component analysis (PCA) to find the best-fit
	 * plane. The plane is oriented so its normal points toward the
	 * viewer (z-component > 0) and is centered on the cloud centroid.
	 *
	 * @param[in] cloud Point cloud (any GenericIndexedCloudPersist)
	 * @param[out] rms Optional RMS fitting error
	 * @return New ccPlane, or nullptr on failure
	 */
	static ccPlane* Fit(CCCoreLib::GenericIndexedCloudPersist* cloud, double* rms = nullptr);

	/**
	 * @brief Get the plane equation as normal + constant
	 *
	 * Returns N and constVal such that: dot(N, P) + constVal = 0
	 * for any point P on the plane.
	 *
	 * @param[out] N Normal vector (set to column 2 of transform)
	 * @param[out] constVal Constant term
	 */
	void getEquation(CCVector3& N, PointCoordinateType& constVal) const;

	/**
	 * @brief Get the plane equation as [a, b, c, d]
	 *
	 * Returns ax + by + cz = d, equivalent to dot(N, P) = d
	 * where N is the normalized normal and d = dot(N, center).
	 *
	 * @return Pointer to [a, b, c, d] array (m_PlaneEquation)
	 */
	const PointCoordinateType* getEquation();

	/**
	 * @brief Flip the plane normal (rotate 180° around X axis)
	 */
	void flip();

  protected:
	// ccDrawableObject
	void drawMeOnly(CC_DRAW_CONTEXT& context) override;

	// ccGenericPrimitive
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	bool  buildUp() override;

	//! Width along local X axis
	PointCoordinateType m_xWidth;
	//! Height along local Y axis
	PointCoordinateType m_yWidth;
	//! Plane equation [a, b, c, d] such that ax + by + cz = d
	PointCoordinateType m_PlaneEquation[4];
};

#endif // CC_PLANE_PRIMITIVE_HEADER
