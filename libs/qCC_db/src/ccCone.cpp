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

#include "ccCone.h"

// Local
#include "ccNormalVectors.h"
#include "ccPointCloud.h"

// ccCone::ccCone
/**
 * @brief Construct a cone or frustum
 *
 * @param[in] bottomRadius Radius at Z=-height/2
 * @param[in] topRadius Radius at Z=+height/2 (can be 0 for pointed tip)
 * @param[in] height Z extent
 * @param[in] xOff X-axis snout offset (displaces top center)
 * @param[in] yOff Y-axis snout offset (displaces top center)
 * @param[in] transMat Optional transformation matrix
 * @param[in] name Display name
 * @param[in] precision Angular tessellation steps
 * @param[in] uniqueID Optional unique ID
 */
ccCone::ccCone(PointCoordinateType bottomRadius,
               PointCoordinateType topRadius,
               PointCoordinateType height,
               PointCoordinateType xOff /*=0*/,
               PointCoordinateType yOff /*=0*/,
               const ccGLMatrix*   transMat /*=nullptr*/,
               QString             name /*="Cylinder"*/,
               unsigned            precision /*=DEFAULT_DRAWING_PRECISION*/,
               unsigned            uniqueID /*=ccUniqueIDGenerator::InvalidUniqueID*/)

    : ccGenericPrimitive(name, transMat, uniqueID)
    , m_bottomRadius(std::abs(bottomRadius))
    , m_topRadius(std::abs(topRadius))
    , m_xOff(xOff)
    , m_yOff(yOff)
    , m_height(std::abs(height))
{
	setDrawingPrecision(std::max<unsigned>(precision, MIN_DRAWING_PRECISION)); // automatically calls buildUp & applyTransformationToVertices
}

// ccCone::ccCone
/**
 * @brief Simplified constructor for ccHObject factory
 *
 * @param[in] name Display name
 */
ccCone::ccCone(QString name /*="Cylinder"*/)
    : ccGenericPrimitive(name)
    , m_bottomRadius(0)
    , m_topRadius(0)
    , m_xOff(0)
    , m_yOff(0)
    , m_height(0)
{
}

// ccCone::clone
/**
 * @brief Clone this cone
 *
 * @return Pointer to the cloned cone
 */
ccGenericPrimitive* ccCone::clone() const
{
	return finishCloneJob(new ccCone(m_bottomRadius, m_topRadius, m_height, m_xOff, m_yOff, &m_transformation, getName(), m_drawPrecision));
}

// ccCone::buildUp
/**
 * @brief Build the tessellated cone/frustum mesh
 *
 * Creates a mesh with configurable geometry:
 *
 * Vertex layout:
 * - Vertex 0: bottom center
 * - Vertex 1: top center
 * - Vertices 2...(steps): bottom ring vertices (if bottomRadius > 0)
 * - Remaining vertices: top ring vertices (if topRadius > 0)
 * - After ring vertices: side normals (steps entries)
 *
 * Face structure:
 * - Bottom disk: fan of triangles from bottom center to bottom ring
 * - Top disk: fan of triangles from top center to top ring (if topRadius > 0)
 * - Lateral surface: quads from bottom ring to top ring (if both > 0),
 *   or triangles from ring to apex (if one radius is 0)
 *
 * Normal computation:
 * - Bottom disk: all -Z
 * - Top disk: all +Z
 * - Lateral surface: perpendicular to the sloping surface, computed
 *   from the cross product of the radial direction and the slope vector
 *
 * Snout mode: the top center is displaced by (m_xOff, m_yOff) from
 * the bottom center, creating an offset frustum.
 *
 * @return true if the mesh was built successfully
 */
bool ccCone::buildUp()
{
	if (m_drawPrecision < MIN_DRAWING_PRECISION)
		return false;

	// Invalid dimensions: zero height or zero sum of radii
	if (CCCoreLib::LessThanEpsilon(m_height) || CCCoreLib::LessThanEpsilon(m_bottomRadius + m_topRadius))
	{
		return false;
	}

	// Degenerate cases: one radius is zero (pointed cone)
	bool singlePointBottom = CCCoreLib::LessThanEpsilon(m_bottomRadius);
	bool singlePointTop    = CCCoreLib::LessThanEpsilon(m_topRadius);
	assert(!singlePointBottom || !singlePointTop); // both can't be zero

	unsigned steps = m_drawPrecision;

	// Pre-compute counts
	unsigned vertCount = 2; // center points
	if (!singlePointBottom)
		vertCount += steps;
	if (!singlePointTop)
		vertCount += steps;
	unsigned faceNormCounts = steps + 2; // side normals + 2 disk normals
	unsigned facesCount = steps; // lateral surface
	if (!singlePointBottom)
		facesCount += steps;
	if (!singlePointTop)
		facesCount += steps;
	if (!singlePointBottom && !singlePointTop)
		facesCount += steps;

	if (!init(vertCount, false, facesCount, faceNormCounts))
	{
		ccLog::Error("[ccCone::buildUp] Not enough memory");
		return false;
	}

	ccPointCloud* verts = vertices();
	assert(verts);
	assert(m_triNormals);

	// 2 first vertices: centers of the top & bottom disks
	// In local space: bottom at z=-height/2, top at z=+height/2
	CCVector3 bottomCenter = CCVector3(m_xOff, m_yOff, -m_height) / 2;
	CCVector3 topCenter    = CCVector3(-m_xOff, -m_yOff, m_height) / 2;
	{
		verts->addPoint(bottomCenter);
		m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, 0, -1).u));

		verts->addPoint(topCenter);
		m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, 0, 1).u));
	}

	// Angular sweep for top and/or bottom rings
	{
		PointCoordinateType angle_rad_step = static_cast<PointCoordinateType>(2.0 * M_PI) / static_cast<PointCoordinateType>(steps);

		// Bottom ring vertices
		if (!singlePointBottom)
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				CCVector3 P(bottomCenter.x + cos(angle_rad_step * i) * m_bottomRadius,
				            bottomCenter.y + sin(angle_rad_step * i) * m_bottomRadius,
				            bottomCenter.z);
				verts->addPoint(P);
			}
		}

		// Top ring vertices
		if (!singlePointTop)
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				CCVector3 P(topCenter.x + cos(angle_rad_step * i) * m_topRadius,
				            topCenter.y + sin(angle_rad_step * i) * m_topRadius,
				            topCenter.z);
				verts->addPoint(P);
			}
		}

		// Lateral surface normals: perpendicular to the sloping surface
		// The normal is perpendicular to both the tangent vector u (angular direction)
		// and the slope vector v (from top to bottom along the sloping side)
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				// u = tangent to the ring (perpendicular to radius in XY plane)
				CCVector3 u(-sin(angle_rad_step * i), cos(angle_rad_step * i), 0);
				// v = vector from top center to bottom center, adjusted for radius difference
				CCVector3 v(bottomCenter.x - topCenter.x + u.y * (m_bottomRadius - m_topRadius),
				            bottomCenter.y - topCenter.y - u.x * (m_bottomRadius - m_topRadius),
				            bottomCenter.z - topCenter.z);
				// N = cross(v, u) = outward normal (right-hand rule)
				CCVector3 N = v.cross(u);
				N.normalize();

				m_triNormals->addElement(ccNormalVectors::GetNormIndex(N.u));
			}
		}
	}

	// Build faces
	{
		assert(m_triVertIndexes);

		unsigned bottomIndex = 2;
		unsigned topIndex    = 2 + (singlePointBottom ? 0 : steps);

		// Bottom disk fan
		if (!singlePointBottom)
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				addTriangle(0, bottomIndex + (i + 1) % steps, bottomIndex + i);
				addTriangleNormalIndexes(0, 0, 0);
			}
		}

		// Top disk fan
		if (!singlePointTop)
		{
			for (unsigned i = 0; i < steps; ++i)
			{
				addTriangle(1, topIndex + i, topIndex + (i + 1) % steps);
				addTriangleNormalIndexes(1, 1, 1);
			}
		}

		// Lateral surface: quad strips
		if (!singlePointBottom && !singlePointTop)
		{
			// Frustum: each quad split into 2 triangles
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned iNext = (i + 1) % steps;
				addTriangle(bottomIndex + i, bottomIndex + iNext, topIndex + i);
				addTriangleNormalIndexes(2 + i, 2 + iNext, 2 + i);
				addTriangle(topIndex + i, bottomIndex + iNext, topIndex + iNext);
				addTriangleNormalIndexes(2 + i, 2 + iNext, 2 + iNext);
			}
		}
		else if (!singlePointTop)
		{
			// Pointed at top: triangles from top center to bottom ring
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned iNext = (i + 1) % steps;
				addTriangle(topIndex + i, 0, topIndex + iNext);
				addTriangleNormalIndexes(2 + i, 2 + iNext, 2 + iNext);
			}
		}
		else // if (!singlePointBottom)
		{
			// Pointed at bottom: triangles from bottom center to top ring
			for (unsigned i = 0; i < steps; ++i)
			{
				unsigned iNext = (i + 1) % steps;
				addTriangle(bottomIndex + i, bottomIndex + iNext, 1);
				addTriangleNormalIndexes(2 + i, 2 + iNext, 2 + i);
			}
		}
	}

	notifyGeometryUpdate();
	showTriNorms(true);

	return true;
}

// ccCone::setHeight
/**
 * @brief Set the cone height
 *
 * @param[in] height New height (must be > 0)
 */
void ccCone::setHeight(PointCoordinateType height)
{
	if (m_height == height)
		return;
	assert(height > 0);
	m_height = height;
	buildUp();
	applyTransformationToVertices();
}

// ccCone::setBottomRadius
/**
 * @brief Set the bottom radius
 *
 * @param[in] radius New bottom radius (must be > 0)
 */
void ccCone::setBottomRadius(PointCoordinateType radius)
{
	if (m_bottomRadius == radius)
		return;
	assert(radius > 0);
	m_bottomRadius = radius;
	buildUp();
	applyTransformationToVertices();
}

// ccCone::setTopRadius
/**
 * @brief Set the top radius
 *
 * @param[in] radius New top radius (can be 0 for pointed tip)
 */
void ccCone::setTopRadius(PointCoordinateType radius)
{
	if (m_topRadius == radius)
		return;
	assert(radius > 0);
	m_topRadius = radius;
	buildUp();
	applyTransformationToVertices();
}

// ccCone::getBottomCenter
/**
 * @brief Get the bottom center in world space
 *
 * @return Bottom center (local: (m_xOff/2, m_yOff/2, -height/2)) transformed to world
 */
CCVector3 ccCone::getBottomCenter() const
{
	CCVector3  bottomCenter = CCVector3(m_xOff, m_yOff, -m_height) / 2;
	ccGLMatrix trans        = getGLTransformationHistory();
	trans.apply(bottomCenter);
	return bottomCenter;
}

// ccCone::getTopCenter
/**
 * @brief Get the top center in world space
 *
 * @return Top center (local: (-m_xOff/2, -m_yOff/2, +height/2)) transformed to world
 */
CCVector3 ccCone::getTopCenter() const
{
	CCVector3  topCenter = CCVector3(-m_xOff, -m_yOff, m_height) / 2;
	ccGLMatrix trans     = getGLTransformationHistory();
	trans.apply(topCenter);
	return topCenter;
}

// ccCone::getSmallCenter
/**
 * @brief Get the center of the smaller-radius end cap
 *
 * @return Bottom center if bottomRadius <= topRadius, else top center
 */
CCVector3 ccCone::getSmallCenter() const
{
	return (m_topRadius <= m_bottomRadius) ? getTopCenter() : getBottomCenter();
}

// ccCone::getLargeCenter
/**
 * @brief Get the center of the larger-radius end cap
 *
 * @return Top center if topRadius >= bottomRadius, else bottom center
 */
CCVector3 ccCone::getLargeCenter() const
{
	return (m_topRadius >= m_bottomRadius) ? getTopCenter() : getBottomCenter();
}

// ccCone::getSmallRadius
/**
 * @brief Get the smaller of the two radii
 */
PointCoordinateType ccCone::getSmallRadius() const
{
	return (m_topRadius <= m_bottomRadius) ? m_topRadius : m_bottomRadius;
}

// ccCone::getLargeRadius
/**
 * @brief Get the larger of the two radii
 */
PointCoordinateType ccCone::getLargeRadius() const
{
	return (m_topRadius >= m_bottomRadius) ? m_topRadius : m_bottomRadius;
}

// ccCone::toFile_MeOnly
/**
 * @brief Serialize to binary file (dataVersion >= 21)
 */
bool ccCone::toFile_MeOnly(QFile& out, short dataVersion) const
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
	outStream << m_bottomRadius;
	outStream << m_topRadius;
	outStream << m_xOff;
	outStream << m_yOff;
	outStream << m_height;

	return true;
}

// ccCone::fromFile_MeOnly
/**
 * @brief Deserialize from binary file (dataVersion >= 21)
 */
bool ccCone::fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap)
{
	if (!ccGenericPrimitive::fromFile_MeOnly(in, dataVersion, flags, oldToNewIDMap))
		return false;

	QDataStream inStream(&in);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_bottomRadius);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_topRadius);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_xOff);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_yOff);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, &m_height);

	return true;
}

// ccCone::minimumFileVersion_MeOnly
/**
 * @brief Minimum file version for this class
 *
 * @return 21
 */
short ccCone::minimumFileVersion_MeOnly() const
{
	return std::max(static_cast<short>(21), ccGenericPrimitive::minimumFileVersion_MeOnly());
}

// ccCone::computeApex
/**
 * @brief Compute the cone apex position
 *
 * For pointed cones (one radius is 0): returns the center of the pointed end.
 * For frustums (both radii > 0): extrapolates the sloping side to the apex
 * by projecting the small radius point along the slope direction.
 *
 * @return Apex position in world space
 */
CCVector3 ccCone::computeApex() const
{
	PointCoordinateType smallRadius = getSmallRadius();
	if (CCCoreLib::LessThanEpsilon(smallRadius))
	{
		// Cone is pointed: apex is at the zero-radius end
		return getTopCenter();
	}

	// For frustums: extrapolate along the slope
	CCVector3 smallCenter = getTopCenter();
	CCVector3 largeCenter = getBottomCenter();
	if (smallRadius == m_bottomRadius)
	{
		std::swap(smallCenter, largeCenter);
	}

	PointCoordinateType deltaRadius = getLargeRadius() - smallRadius;
	CCVector3           slope       = (smallCenter - largeCenter) / std::max(deltaRadius, std::numeric_limits<PointCoordinateType>::quiet_NaN());

	return smallCenter + smallRadius * slope;
}

// ccCone::computeHalfAngle_deg
/**
 * @brief Compute the cone half angle
 *
 * Returns the angle between the cone axis and the sloping surface,
 * in degrees. For a right circular cone: atan2(deltaRadius, height).
 *
 * @return Half angle in degrees
 */
double ccCone::computeHalfAngle_deg() const
{
	double height      = (getTopCenter() - getBottomCenter()).normd();
	double deltaRadius = getLargeRadius() - getSmallRadius();

	return CCCoreLib::RadiansToDegrees(atan2(deltaRadius, height));
}
