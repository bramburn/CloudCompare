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

#include "ccDish.h"

// Local
#include "ccNormalVectors.h"
#include "ccPointCloud.h"

// ccDish::ccDish
/**
 * @brief Construct a dish
 *
 * @param[in] radius Base radius (the circular opening)
 * @param[in] height Height above the base
 * @param[in] radius2 Second radius for ellipsoidal variant (0 = spherical)
 * @param[in] transMat Optional transformation matrix
 * @param[in] name Display name
 * @param[in] precision Angular tessellation steps
 */
ccDish::ccDish(PointCoordinateType radius,
               PointCoordinateType height,
               PointCoordinateType radius2 /*=0*/,
               const ccGLMatrix* transMat /*=nullptr*/,
               QString name /*="Dish"*/,
               unsigned precision /*=DEFAULT_DRAWING_PRECISION*/)
    : ccGenericPrimitive(name, transMat)
    , m_baseRadius(radius)
    , m_secondRadius(radius2)
    , m_height(height)
{
	// In spherical mode, height is clamped to radius (dishes are at most hemispheres)
	if (radius2 == 0)
		m_height = std::min(height, radius);
	setDrawingPrecision(std::max<unsigned>(precision, MIN_DRAWING_PRECISION)); // automatically calls buildUp + applyTransformationToVertices
}

// ccDish::ccDish
/**
 * @brief Simplified constructor for ccHObject factory
 *
 * @param[in] name Display name
 */
ccDish::ccDish(QString name /*="Sphere"*/)
    : ccGenericPrimitive(name)
    , m_baseRadius(0)
    , m_secondRadius(0)
    , m_height(0)
{
}

// ccDish::clone
/**
 * @brief Clone this dish
 */
ccGenericPrimitive* ccDish::clone() const
{
	return finishCloneJob(new ccDish(m_baseRadius, m_height, m_secondRadius, &m_transformation, getName(), m_drawPrecision));
}

// ccDish::buildUp
/**
 * @brief Build the dish mesh
 *
 * Creates a spherical cap mesh with the following structure:
 *
 * Geometry (spherical mode, m_secondRadius == 0):
 * - A section of a sphere of radius realRadius
 * - Base at z = 0 (flat disk)
 * - Dome rises to height m_height
 * - North pole at z = m_height
 *
 * The sphere's origin is displaced from the dish's base so the
 * cap has the desired height. For partial spherical caps
 * (height < radius), the cap starts at angle startAngle from the
 * north pole; for hemispheres (height >= radius), it starts at 0.
 *
 * Geometry (ellipsoidal mode, m_secondRadius > 0):
 * - Section of an ellipsoid with radii (m_baseRadius, m_secondRadius, m_height)
 * - Base at z = 0, north pole at z = m_height
 *
 * Vertex layout:
 * - Vertex 0: north pole
 * - Vertices 1...: rings from top to bottom (sectionSteps rings, steps vertices per ring)
 *
 * The angular span is always 0 to π/2 (north pole to equator).
 * For partial spherical caps: startAngle > 0 (skip the lower portion).
 *
 * @return true if the mesh was built successfully
 */
bool ccDish::buildUp()
{
	if (m_drawPrecision < MIN_DRAWING_PRECISION)
		return false;

	if (m_height <= 0 || m_baseRadius <= 0 || m_secondRadius < 0)
		return false;

	// Angular span: from north pole (0) to equator (π/2)
	double startAngle_rad = 0.0;
	const double endAngle_rad = M_PI / 2.0;

	PointCoordinateType realRadius = m_baseRadius;
	if (m_secondRadius == 0 && m_height < m_baseRadius)
	{
		// Partial spherical cap: sphere origin is displaced so the cap has height h
		// realRadius = (h² + r²) / 2h
		// startAngle = acos(r / realRadius)
		realRadius = (m_height * m_height + m_baseRadius * m_baseRadius) / (2 * m_height);
		startAngle_rad = acos(m_baseRadius / realRadius);
		assert(startAngle_rad < endAngle_rad);
	}

	const unsigned steps = m_drawPrecision;
	double angleStep_rad = 2.0 * M_PI / steps;
	// sectionSteps: number of angular steps in the theta direction (0 to π/2)
	unsigned sectionSteps = static_cast<unsigned>(ceil((endAngle_rad - startAngle_rad) * m_drawPrecision / (2.0 * M_PI)));
	double sectionAngleStep_rad = (endAngle_rad - startAngle_rad) / sectionSteps;

	// Pre-compute counts: 1 north pole + sectionSteps * steps ring vertices
	unsigned vertCount = steps * sectionSteps + 1;
	unsigned faceCount = steps * ((sectionSteps - 1) * 2 + 1);

	if (!init(vertCount, true, faceCount, 0))
	{
		ccLog::Error("[ccDish::buildUp] Not enough memory");
		return false;
	}

	ccPointCloud* verts = vertices();
	assert(verts);

	// Vertex 0: north pole at (0, 0, m_height)
	verts->addPoint(CCVector3(0, 0, m_height));
	verts->addNorm(CCVector3(0, 0, 1));

	// Ring vertices: sweep from north pole toward base
	{
		for (unsigned j = 1; j <= sectionSteps; ++j)
		{
			// theta: angle from north pole, increasing toward equator
			PointCoordinateType theta = static_cast<PointCoordinateType>(endAngle_rad - j * sectionAngleStep_rad);
			PointCoordinateType cos_theta = cos(theta);
			PointCoordinateType sin_theta = sin(theta);

			// Direction vector from sphere center to surface point
			CCVector3 N0(cos_theta, 0, sin_theta);

			for (unsigned i = 0; i < steps; ++i)
			{
				// phi: azimuthal angle (full revolution around Z)
				PointCoordinateType phi = static_cast<PointCoordinateType>(i * angleStep_rad);
				PointCoordinateType cos_phi = cos(phi);
				PointCoordinateType sin_phi = sin(phi);

				// Normal: rotated around Z by phi
				CCVector3 N(N0.x * cos_phi, N0.x * sin_phi, N0.z);
				N.normalize();

				// Vertex position
				CCVector3 P = N * realRadius;

				if (m_secondRadius > 0)
				{
					// Ellipsoidal: scale Y by secondRadius/baseRadius, Z by height/baseRadius
					P.y *= (m_secondRadius / m_baseRadius);
					P.z *= (m_height / m_baseRadius);
				}
				else
				{
					// Spherical: offset so base is at z=0
					P.z += m_height - realRadius;
				}

				verts->addPoint(P);
				verts->addNorm(N);
			}
		}
	}

	// Build faces: north pole fan + ring quads
	{
		// North pole fan
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned A = 1 + i;
				unsigned B = (i + 1 < steps ? A + 1 : 1);
				addTriangle(A, B, 0);
			}
		}

		// Ring quads: each quad split into 2 triangles
		for (unsigned j = 1; j < sectionSteps; ++j)
		{
			unsigned shift = 1 + (j - 1) * steps;
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned A = shift + i;
				unsigned B = (i + 1 < steps ? A + 1 : shift);
				assert(B < vertCount);
				addTriangle(A, A + steps, B);
				addTriangle(B + steps, B, A + steps);
			}
		}
	}

	notifyGeometryUpdate();
	showNormals(true);

	return true;
}

// ccDish::toFile_MeOnly
/**
 * @brief Serialize to binary file (dataVersion >= 21)
 */
bool ccDish::toFile_MeOnly(QFile& out, short dataVersion) const
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
	outStream << m_baseRadius;
	outStream << m_secondRadius;
	outStream << m_height;

	return true;
}

// ccDish::fromFile_MeOnly
/**
 * @brief Deserialize from binary file (dataVersion >= 21)
 */
bool ccDish::fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap)
{
	if (!ccGenericPrimitive::fromFile_MeOnly(in, dataVersion, flags, oldToNewIDMap))
		return false;

	QDataStream inStream(&in);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_baseRadius);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_secondRadius);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_height);

	return true;
}

// ccDish::minimumFileVersion_MeOnly
/**
 * @brief Minimum file version for this class
 *
 * @return 21
 */
short ccDish::minimumFileVersion_MeOnly() const
{
	return std::max(static_cast<short>(21), ccGenericPrimitive::minimumFileVersion_MeOnly());
}
