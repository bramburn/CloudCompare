// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#include "ccPlane.h"

#include "ccIncludeGL.h"
#include "ccMaterialSet.h"
#include "ccPointCloud.h"

#include <DistanceComputationTools.h>
#include <Neighbourhood.h>

// ccPlane::ccPlane
/**
 * @brief Construct a plane with specified dimensions
 *
 * Calls updateRepresentation() (via setDrawingPrecision in the base class)
 * to build the mesh immediately.
 *
 * @param[in] xWidth Width along local X axis
 * @param[in] yWidth Height along local Y axis
 * @param[in] transMat Optional transformation (position + orientation)
 * @param[in] name Display name
 */
ccPlane::ccPlane(PointCoordinateType xWidth, PointCoordinateType yWidth, const ccGLMatrix* transMat /*=nullptr*/, QString name /*=QString("Plane")*/)
    : ccGenericPrimitive(name, transMat)
    , m_xWidth(xWidth)
    , m_yWidth(yWidth)
{
	updateRepresentation();
}

// ccPlane::ccPlane
/**
 * @brief Simplified constructor for ccHObject factory
 *
 * @param[in] name Display name
 */
ccPlane::ccPlane(QString name /*=QString("Plane")*/)
    : ccGenericPrimitive(name)
    , m_xWidth(0)
    , m_yWidth(0)
{
}

// ccPlane::buildUp
/**
 * @brief Build the plane mesh
 *
 * Creates a 4-vertex, 2-triangle quad in the local XY plane:
 *
 *     B(xw/2, yw/2, 0) ------ C(xw/2, -yw/2, 0)
 *        |                        |
 *        |                        |
 *     A(-xw/2, yw/2, 0) ------ D(-xw/2, -yw/2, 0)
 *
 * Triangle 1: A(0), C(2), B(1)   [uses diagonal B-C]
 * Triangle 2: A(0), D(3), C(2)  [uses diagonal D-C]
 *
 * The normal is +Z (0,0,1) in local space and is the same for
 * both triangles (flat shading via shared normal).
 *
 * @return true if the mesh was built successfully
 */
bool ccPlane::buildUp()
{
	if (!init(4, false, 2, 1))
	{
		ccLog::Error("[ccPlane::buildUp] Not enough memory");
		return false;
	}

	ccPointCloud* verts = vertices();
	assert(verts);
	assert(m_triNormals);

	// Vertices: center at origin in local space
	// B ------ C
	// |        |
	// A ------ D
	verts->addPoint(CCVector3(-m_xWidth / 2, -m_yWidth / 2, 0)); // A
	verts->addPoint(CCVector3(-m_xWidth / 2, m_yWidth / 2, 0));  // B
	verts->addPoint(CCVector3(m_xWidth / 2, m_yWidth / 2, 0));   // C
	verts->addPoint(CCVector3(m_xWidth / 2, -m_yWidth / 2, 0));  // D

	// Normal: +Z in local space
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, 0, 1)));

	// Triangle 1: A C B (diagonal B-C)
	addTriangle(0, 2, 1);
	addTriangleNormalIndexes(0, 0, 0);

	// Triangle 2: A D C (diagonal D-C)
	addTriangle(0, 3, 2);
	addTriangleNormalIndexes(0, 0, 0);

	return true;
}

// ccPlane::clone
/**
 * @brief Clone this plane
 *
 * @return Pointer to the cloned plane
 */
ccGenericPrimitive* ccPlane::clone() const
{
	return finishCloneJob(new ccPlane(m_xWidth, m_yWidth, &m_transformation, getName()));
}

// ccPlane::drawMeOnly
/**
 * @brief Draw the plane
 *
 * Calls the parent ccGenericPrimitive::drawMeOnly() to render the mesh,
 * then optionally draws the normal vector if normalVectorIsShown().
 *
 * The normal arrow is scaled by the geometric mean of the plane dimensions.
 *
 * @param[in] context OpenGL drawing context
 */
void ccPlane::drawMeOnly(CC_DRAW_CONTEXT& context)
{
	// call parent method
	ccGenericPrimitive::drawMeOnly(context);

	// show normal vector
	if (MACRO_Draw3D(context) && normalVectorIsShown())
	{
		// Scale: geometric mean of width and height / 2
		PointCoordinateType scale = static_cast<PointCoordinateType>(sqrt(static_cast<double>(m_xWidth) * m_yWidth) / 2);
		glDrawNormal(context, m_transformation.getTranslationAsVec3D(), scale);
	}
}

// ccPlane::getEquation
/**
 * @brief Get the plane equation as normal + constant
 *
 * Returns N and constVal such that: dot(N, P) + constVal = 0
 * for any point P on the plane.
 *
 * The normal is the local Z axis rotated by the transformation matrix.
 *
 * @param[out] N Plane normal (unit vector, world space)
 * @param[out] constVal Constant term: -dot(N, center)
 */
void ccPlane::getEquation(CCVector3& N, PointCoordinateType& constVal) const
{
	N = CCVector3(0, 0, 1);
	m_transformation.applyRotation(N);

	constVal = m_transformation.getTranslationAsVec3D().dot(N);
}

// ccPlane::getEquation
/**
 * @brief Get the plane equation as [a, b, c, d]
 *
 * Fills m_PlaneEquation with: ax + by + cz = d
 * where [a,b,c] is the unit normal and d = dot(N, center).
 *
 * @return Pointer to m_PlaneEquation[4]
 */
const PointCoordinateType* ccPlane::getEquation()
{
	CCVector3 N = getNormal();
	m_PlaneEquation[0] = N.x;
	m_PlaneEquation[1] = N.y;
	m_PlaneEquation[2] = N.z;
	m_PlaneEquation[3] = getCenter().dot(N); // dot(normal, any_point_on_plane)
	return m_PlaneEquation;
}

// ccPlane::Fit
/**
 * @brief Fit a plane to a point cloud
 *
 * Uses principal component analysis (PCA) via CCCoreLib::Neighbourhood
 * to compute the least-squares best-fit plane:
 * 1. Compute centroid G of the point cloud
 * 2. Build local orthonormal basis (X, Y, N) from the PCA
 * 3. Project all points to the 2D plane and find axis-aligned bounding box
 * 4. Create a ccPlane centered at the bounding box center with the
 *    bounding box dimensions as width and height
 *
 * The transformation matrix encodes:
 * - X = main direction (largest eigenvalue)
 * - Y = N × X (orthogonal to N and X)
 * - N = normal (smallest eigenvalue)
 * - Translation = bounding box center
 *
 * The plane normal is oriented so its Z component is positive.
 *
 * Optionally computes the RMS fitting error via
 * DistanceComputationTools::computeCloud2PlaneDistanceRMS() and
 * stores it in the plane's metadata as "RMS".
 *
 * @param[in] cloud Point cloud (at least 3 points)
 * @param[out] rms Optional RMS fitting error
 * @return New ccPlane, or nullptr if fitting fails
 */
ccPlane* ccPlane::Fit(CCCoreLib::GenericIndexedCloudPersist* cloud, double* rms /*=nullptr*/)
{
	unsigned count = cloud->size();
	if (count < 3)
	{
		ccLog::Warning("[ccPlane::Fit] Not enough points in input cloud to fit a plane!");
		return nullptr;
	}

	CCCoreLib::Neighbourhood Yk(cloud);

	const PointCoordinateType* theLSPlane = Yk.getLSPlane();
	if (!theLSPlane)
	{
		ccLog::Warning("[ccPlane::Fit] Not enough points to fit a plane!");
		return nullptr;
	}

	const CCVector3* G = Yk.getGravityCenter();
	assert(G);

	// Local orthonormal basis: N = normal, X = main direction, Y = N × X
	CCVector3 N(theLSPlane);
	const CCVector3* X = Yk.getLSPlaneX();
	assert(X);
	CCVector3 Y = N * (*X);

	// Project points onto the 2D plane and find axis-aligned bounding box
	CCVector2 minXY(0, 0);
	CCVector2 maxXY(0, 0);
	cloud->placeIteratorAtBeginning();
	for (unsigned k = 0; k < count; ++k)
	{
		CCVector3 P = *(cloud->getNextPoint()) - *G;
		CCVector2 P2D(P.dot(*X), P.dot(Y));

		if (k != 0)
		{
			if (minXY.x > P2D.x)
				minXY.x = P2D.x;
			else if (maxXY.x < P2D.x)
				maxXY.x = P2D.x;
			if (minXY.y > P2D.y)
				minXY.y = P2D.y;
			else if (maxXY.y < P2D.y)
				maxXY.y = P2D.y;
		}
		else
		{
			minXY = maxXY = P2D;
		}
	}

	// Recenter on the bounding box center
	PointCoordinateType dX = maxXY.x - minXY.x;
	PointCoordinateType dY = maxXY.y - minXY.y;
	CCVector3 Gt = *G + *X * (minXY.x + dX / 2) + Y * (minXY.y + dY / 2);
	ccGLMatrix glMat(*X, Y, N, Gt);

	ccPlane* plane = new ccPlane(dX, dY, &glMat);

	if (rms)
	{
		*rms = CCCoreLib::DistanceComputationTools::computeCloud2PlaneDistanceRMS(cloud, theLSPlane);
		assert(std::isfinite(*rms));
		plane->setMetaData("RMS", *rms);
	}

	return plane;
}

// ccPlane::toFile_MeOnly
/**
 * @brief Serialize to binary file
 *
 * Writes m_xWidth and m_yWidth (dataVersion >= 21).
 *
 * @param[out] out Open file for writing
 * @param[in] dataVersion File version
 * @return true on success
 */
bool ccPlane::toFile_MeOnly(QFile& out, short dataVersion) const
{
	assert(out.isOpen() && (out.openMode() & QIODevice::WriteOnly));
	if (dataVersion < 21)
	{
		assert(false);
		return false;
	}

	if (!ccGenericPrimitive::toFile_MeOnly(out, dataVersion))
	{
		return false;
	}

	QDataStream outStream(&out);
	outStream << m_xWidth;
	outStream << m_yWidth;

	return true;
}

// ccPlane::fromFile_MeOnly
/**
 * @brief Deserialize from binary file
 *
 * Reads m_xWidth and m_yWidth (dataVersion >= 21).
 *
 * @param[in] in Open file for reading
 * @param[in] dataVersion File version
 * @param[in] flags Serialization flags
 * @param[in] oldToNewIDMap ID remapping table
 * @return true on success
 */
bool ccPlane::fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap)
{
	if (!ccGenericPrimitive::fromFile_MeOnly(in, dataVersion, flags, oldToNewIDMap))
		return false;

	QDataStream inStream(&in);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_xWidth, 1);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_yWidth, 1);

	return true;
}

// ccPlane::minimumFileVersion_MeOnly
/**
 * @brief Minimum file version for this class
 *
 * @return 21 (version when xWidth/yWidth were introduced)
 */
short ccPlane::minimumFileVersion_MeOnly() const
{
	return std::max(static_cast<short>(21), ccGenericPrimitive::minimumFileVersion_MeOnly());
}

// ccPlane::getOwnFitBB
/**
 * @brief Get the plane's fit bounding box
 *
 * Returns the axis-aligned bounding box in local space: from
 * (-xWidth/2, -yWidth/2, 0) to (xWidth/2, yWidth/2, 0).
 *
 * @param[out] trans Transformation matrix (identity in local space)
 * @return The bounding box
 */
ccBBox ccPlane::getOwnFitBB(ccGLMatrix& trans)
{
	trans = m_transformation;
	return ccBBox(CCVector3(-m_xWidth / 2, -m_yWidth / 2, 0), CCVector3(m_xWidth / 2, m_yWidth / 2, 0), true);
}

// ccPlane::setAsTexture
/**
 * @brief Apply a texture image to the plane
 *
 * Delegates to SetQuadTexture().
 *
 * @param[in] image QImage to map onto the plane
 * @param[in] imageFilename Optional filename
 * @return The created material, or nullptr on failure
 */
ccMaterial::Shared ccPlane::setAsTexture(QImage image, QString imageFilename /*=QString()*/)
{
	return SetQuadTexture(this, image, imageFilename);
}

// ccPlane::SetQuadTexture
/**
 * @brief Apply a texture to a quad mesh
 *
 * Sets up the full texture pipeline:
 * 1. Creates or uses existing texture coordinate table (UV mapping)
 * 2. Creates per-triangle texture indexes
 * 3. Creates per-triangle material indexes
 * 4. Creates a ccMaterial with the image texture
 * 5. Enables material display on the mesh
 *
 * UV coordinates assigned to vertices:
 * - Vertex A (-xw/2, -yw/2, 0) → (0, 0) [bottom-left]
 * - Vertex B (-xw/2,  yw/2, 0) → (0, 1) [top-left]
 * - Vertex C ( xw/2,  yw/2, 0) → (1, 1) [top-right]
 * - Vertex D ( xw/2, -yw/2, 0) → (1, 0) [bottom-right]
 *
 * Triangle texture indexes:
 * - Triangle A-C-B → (0, 2, 1)
 * - Triangle A-D-C → (0, 3, 2)
 *
 * @param[in] quadMesh Target mesh (must have exactly 4 vertices)
 * @param[in] image QImage to map
 * @param[in] imageFilename Optional filename
 * @return The created material, or nullptr on failure
 */
ccMaterial::Shared ccPlane::SetQuadTexture(ccMesh* quadMesh, QImage image, QString imageFilename /*=QString()*/)
{
	if (!quadMesh
	    || quadMesh->size() > 2
	    || !quadMesh->getAssociatedCloud()
	    || quadMesh->getAssociatedCloud()->size() > 4)
	{
		ccLog::Warning("[ccPlane::SetQuadTexture] Invalid input quad");
		return nullptr;
	}

	if (image.isNull())
	{
		ccLog::Warning("[ccPlane::SetQuadTexture] Invalid texture image!");
		return ccMaterial::Shared(nullptr);
	}

	// Set up texture coordinates if not already present
	TextureCoordsContainer* texCoords = quadMesh->getTexCoordinatesTable();
	if (!texCoords)
	{
		texCoords = new TextureCoordsContainer();
		if (!texCoords->reserveSafe(4))
		{
			ccLog::Warning("[ccPlane::setAsTexture] Not enough memory!");
			delete texCoords;
			return ccMaterial::Shared(nullptr);
		}

		// UV: A→(0,0), B→(0,1), C→(1,1), D→(1,0)
		texCoords->emplace_back(TexCoords2D(0.0f, 0.0f)); // A
		texCoords->emplace_back(TexCoords2D(0.0f, 1.0f)); // B
		texCoords->emplace_back(TexCoords2D(1.0f, 1.0f)); // C
		texCoords->emplace_back(TexCoords2D(1.0f, 0.0f)); // D

		quadMesh->setTexCoordinatesTable(texCoords);
	}

	// Per-triangle texture indexes
	if (!quadMesh->hasPerTriangleTexCoordIndexes())
	{
		if (!quadMesh->reservePerTriangleTexCoordIndexes())
		{
			ccLog::Warning("[ccPlane::setAsTexture] Not enough memory!");
			quadMesh->setTexCoordinatesTable(nullptr);
			quadMesh->removePerTriangleMtlIndexes();
			return ccMaterial::Shared(nullptr);
		}

		// Triangle 1: A-C-B → (0, 2, 1), Triangle 2: A-D-C → (0, 3, 2)
		quadMesh->addTriangleTexCoordIndexes(0, 2, 1);
		quadMesh->addTriangleTexCoordIndexes(0, 3, 2);
	}

	// Per-triangle material indexes
	if (!quadMesh->hasPerTriangleMtlIndexes())
	{
		if (!quadMesh->reservePerTriangleMtlIndexes())
		{
			ccLog::Warning("[ccPlane::setAsTexture] Not enough memory!");
			quadMesh->setTexCoordinatesTable(nullptr);
			quadMesh->removePerTriangleTexCoordIndexes();
			return ccMaterial::Shared(nullptr);
		}

		quadMesh->addTriangleMtlIndex(0);
		quadMesh->addTriangleMtlIndex(0);
	}

	// Create material with texture
	if (!quadMesh->getMaterialSet())
	{
		quadMesh->setMaterialSet(new ccMaterialSet());
	}
	ccMaterialSet* materialSet = const_cast<ccMaterialSet*>(quadMesh->getMaterialSet());
	assert(materialSet);
	materialSet->clear();
	ccMaterial::Shared material(new ccMaterial("texture"));
	material->setTexture(image, imageFilename, false);
	materialSet->addMaterial(material);

	quadMesh->showMaterials(true);

	return material;
}

// ccPlane::flip
/**
 * @brief Flip the plane normal
 *
 * Rotates the transformation matrix by 180° around the local X axis.
 * The transformation becomes: m_transformation × reverseMat(π, 1,0,0, 0,0,0)
 *
 * This is equivalent to rotating the plane 180° around X, which reverses
 * the Z axis direction (flips the normal from +Z to -Z in local space).
 */
void ccPlane::flip()
{
	ccGLMatrix reverseMat;
	reverseMat.initFromParameters(static_cast<PointCoordinateType>(M_PI), CCVector3(1, 0, 0), CCVector3(0, 0, 0));

	m_transformation = m_transformation * reverseMat;
	updateRepresentation();
}
