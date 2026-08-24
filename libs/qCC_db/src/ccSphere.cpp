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

#include "ccSphere.h"

// Local
#include "ccPointCloud.h"

// QT
#include <QFontMetrics>

// ccSphere::ccSphere
/**
 * @brief Construct a sphere with specified radius
 *
 * Automatically calls buildUp() via setDrawingPrecision() to create
 * the tessellated mesh.
 *
 * @param[in] radius Sphere radius (must be > 0)
 * @param[in] transMat Optional transformation matrix
 * @param[in] name Display name
 * @param[in] precision Angular tessellation steps
 * @param[in] uniqueID Optional unique ID
 */
ccSphere::ccSphere(PointCoordinateType radius,
                   const ccGLMatrix* transMat /*=nullptr*/,
                   QString name /*=QString("Sphere")*/,
                   unsigned precision /*=DEFAULT_DRAWING_PRECISION*/,
                   unsigned uniqueID /*=ccUniqueIDGenerator::InvalidUniqueID*/)
    : ccGenericPrimitive(name, transMat, uniqueID)
    , m_radius(radius)
{
	setDrawingPrecision(std::max<unsigned>(precision, MIN_DRAWING_PRECISION)); // automatically calls updateRepresentation
}

// ccSphere::ccSphere
/**
 * @brief Simplified constructor for ccHObject factory
 *
 * @param[in] name Display name
 */
ccSphere::ccSphere(QString name /*=QString("Sphere")*/)
    : ccGenericPrimitive(name)
    , m_radius(0)
{
}

// ccSphere::clone
/**
 * @brief Clone this sphere
 *
 * Creates a new ccSphere with the same radius, transformation,
 * name, and drawing precision.
 *
 * @return Pointer to the cloned sphere
 */
ccGenericPrimitive* ccSphere::clone() const
{
	return finishCloneJob(new ccSphere(m_radius, &m_transformation, getName(), m_drawPrecision));
}

// ccSphere::buildUp
/**
 * @brief Build the tessellated sphere mesh
 *
 * Creates a UV sphere mesh with the following structure:
 * - 2 vertices at the poles (north: +Z, south: -Z)
 * - (steps-1) rings of vertices between the poles
 * - steps vertices per ring, distributed evenly in phi
 * - Triangular facets connecting adjacent rings
 *
 * Vertex layout:
 * - Vertices 0-1: north and south poles
 * - Vertices 2 to 2+(steps-1)*steps-1: ring vertices
 *
 * Total vertices: steps*(steps-1) + 2
 * Total triangles: steps*(2*steps-4) + 2*steps
 *
 * Normals are computed per-vertex and point radially outward
 * for smooth shading.
 *
 * @return true if mesh was built successfully
 */
bool ccSphere::buildUp()
{
	if (m_drawPrecision < MIN_DRAWING_PRECISION)
		return false;

	const unsigned steps = m_drawPrecision;

	// vertices
	ccPointCloud* verts = vertices();
	assert(verts);

	// Pre-compute total counts
	unsigned count = steps * (steps - 1) + 2;
	unsigned faces = steps * ((steps - 2) * 2 + 2);

	if (!init(count, true, faces, 0))
	{
		ccLog::Error("[ccSphere::buildUp] Not enough memory");
		return false;
	}

	// 2 first points: poles
	verts->addPoint(CCVector3(0, 0, m_radius));
	verts->addNorm(CCVector3(0, 0, 1));

	verts->addPoint(CCVector3(0, 0, -m_radius));
	verts->addNorm(CCVector3(0, 0, -1));

	// Generate ring vertices: latitude (theta) and longitude (phi) sweep
	PointCoordinateType angle_rad_step = static_cast<PointCoordinateType>(M_PI) / static_cast<PointCoordinateType>(steps);
	CCVector3 N0;
	CCVector3 N;
	CCVector3 P;
	{
		for (unsigned j = 1; j < steps; ++j)
		{
			PointCoordinateType theta = static_cast<PointCoordinateType>(j) * angle_rad_step;
			PointCoordinateType cos_theta = cos(theta);
			PointCoordinateType sin_theta = sin(theta);

			// Normal direction for this latitude ring (before longitude rotation)
			N0.x = sin_theta;
			N0.y = 0;
			N0.z = cos_theta;

			for (unsigned i = 0; i < steps; ++i)
			{
				PointCoordinateType phi = static_cast<PointCoordinateType>(2 * i) * angle_rad_step;
				PointCoordinateType cos_phi = cos(phi);
				PointCoordinateType sin_phi = sin(phi);

				// Rotate N0 around Z by phi to get the surface normal
				N.x = N0.x * cos_phi;
				N.y = N0.x * sin_phi;
				N.z = N0.z;
				N.normalize();

				// Vertex position = normal * radius (on sphere surface)
				P = N * m_radius;

				verts->addPoint(P);
				verts->addNorm(N);
			}
		}
	}

	// Build triangular facets
	{
		assert(m_triVertIndexes);

		// North pole cap: fan of triangles to first ring
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned A = 2 + i;
				unsigned B = (i + 1 < steps ? A + 1 : 2);
				addTriangle(A, B, 0);
			}
		}

		// Middle rings: quadrilaterals split into 2 triangles each
		for (unsigned j = 1; j + 1 < steps; ++j)
		{
			unsigned shift = 2 + (j - 1) * steps;
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned A = shift + i;
				unsigned B = (i + 1 < steps ? A + 1 : shift);
				assert(B < count);
				addTriangle(A, A + steps, B);
				addTriangle(B + steps, B, A + steps);
			}
		}

		// South pole cap: fan of triangles from last ring to south pole
		{
			unsigned shift = 2 + (steps - 2) * steps;
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned A = shift + i;
				unsigned B = (i + 1 < steps ? A + 1 : shift);
				assert(B < count);
				addTriangle(A, 1, B);
			}
		}
	}

	notifyGeometryUpdate();
	showNormals(true);

	return true;
}

// ccSphere::setRadius
/**
 * @brief Set the sphere radius
 *
 * Updates the radius and rebuilds the mesh.
 * Also calls applyTransformationToVertices() to update vertex positions.
 *
 * @param[in] radius New radius (must be > 0)
 */
void ccSphere::setRadius(PointCoordinateType radius)
{
	if (m_radius == radius)
		return;

	assert(radius > 0);
	m_radius = radius;

	buildUp();
	applyTransformationToVertices();
}

// ccSphere::toFile_MeOnly
/**
 * @brief Serialize to binary file
 *
 * Writes m_radius to the file (dataVersion >= 21).
 *
 * @param[out] out Open file for writing
 * @param[in] dataVersion File version
 * @return true on success
 */
bool ccSphere::toFile_MeOnly(QFile& out, short dataVersion) const
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

	// parameters (dataVersion >= 21)
	QDataStream outStream(&out);
	outStream << m_radius;

	return true;
}

// ccSphere::fromFile_MeOnly
/**
 * @brief Deserialize from binary file
 *
 * Reads m_radius from the file (dataVersion >= 21).
 *
 * @param[in] in Open file for reading
 * @param[in] dataVersion File version
 * @param[in] flags Serialization flags
 * @param[in] oldToNewIDMap ID remapping table
 * @return true on success
 */
bool ccSphere::fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap)
{
	if (!ccGenericPrimitive::fromFile_MeOnly(in, dataVersion, flags, oldToNewIDMap))
		return false;

	// parameters (dataVersion >= 21)
	QDataStream inStream(&in);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_radius, 1);

	return true;
}

// ccSphere::minimumFileVersion_MeOnly
/**
 * @brief Minimum file version for this class
 *
 * @return 21 (version when m_radius was introduced)
 */
short ccSphere::minimumFileVersion_MeOnly() const
{
	return std::max(static_cast<short>(21), ccGenericPrimitive::minimumFileVersion_MeOnly());
}

// ccSphere::drawNameIn3D
/**
 * @brief Draw the entity name label in 3D view
 *
 * Projects the sphere's center to screen coordinates and displays
 * the name text next to the sphere (offset by the sphere's screen
 * radius so the label appears beside the sphere, not on top of it).
 *
 * @param[in] context OpenGL drawing context
 */
void ccSphere::drawNameIn3D(CC_DRAW_CONTEXT& context)
{
	if (!context.display)
		return;

	ccBBox bBox = getOwnBB();
	if (!bBox.isValid())
		return;

	ccGLMatrix trans;
	getAbsoluteGLTransformation(trans);

	ccGLCameraParameters camera;
	context.display->getGLCameraParameters(camera);

	CCVector3 C = bBox.getCenter();
	CCVector3d Q2D;
	trans.apply(C);
	camera.project(C, Q2D);

	// Offset label horizontally by the sphere's screen radius
	const ccViewportParameters& params = context.display->getViewportParameters();
	int dPix = static_cast<int>(ceil(m_radius / camera.pixelSize));

	int bkgBorder = QFontMetrics(context.display->getTextDisplayFont()).height() / 4 + 4;
	QFont font = context.display->getTextDisplayFont();
	context.display->displayText(getName(),
	                             static_cast<int>(Q2D.x) + dPix + bkgBorder,
	                             static_cast<int>(Q2D.y),
	                             ccGenericGLDisplay::ALIGN_HLEFT | ccGenericGLDisplay::ALIGN_VMIDDLE,
	                             0.75f,
	                             nullptr,
	                             &font);
}
