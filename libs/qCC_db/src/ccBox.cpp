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

/**
 * @file ccBox.cpp
 *
 * @brief Box primitive implementation
 *
 * Implements buildUp() which creates the 8-vertex, 12-triangle,
 * 6-face flat-shaded mesh. Vertices are centered at the origin
 * in local space.
 *
 * @see ccBox.h
 */

#include "ccBox.h"

// qCC_db
#include "ccPlane.h"
#include "ccPointCloud.h"

// ccBox::ccBox
/**
 * @brief Create a box with specified dimensions
 *
 * @param[in] dims Width (X), height (Y), depth (Z)
 * @param[in] transMat Optional transformation matrix
 * @param[in] name Display name
 */
ccBox::ccBox(const CCVector3& dims,
             const ccGLMatrix* transMat /*=nullptr*/,
             QString name /*=QString("Box")*/)
    : ccGenericPrimitive(name, transMat)
    , m_dims(dims)
{
	updateRepresentation();
}

// ccBox::ccBox (factory constructor)
/**
 * @brief Simplified constructor for ccHObject factory
 */
ccBox::ccBox(QString name /*=QString("Box")*/)
    : ccGenericPrimitive(name)
    , m_dims(0, 0, 0)
{
}

// ccBox::buildUp
/**
 * @brief Build the box mesh
 *
 * Allocates 8 vertices and 12 triangles (2 per face).
 * Vertices are positioned at the 8 corners of the cuboid,
 * centered at the origin:
 *
 *     5 -- 6
 *   1/-- 2/|
 *   | 4 -|-7
 *   0/-- 3/
 *
 * Each face gets its own flat-shaded normal (one normal per 2 triangles).
 */
bool ccBox::buildUp()
{
	if (!init(8, false, 12, 6))
	{
		ccLog::Error("[ccPlane::buildUp] Not enough memory");
		return false;
	}

	ccPointCloud* verts = vertices();
	assert(verts);
	assert(m_triNormals);

	// 8 vertices at cuboid corners (centered at origin)
	verts->addPoint(CCVector3(-m_dims.x / 2, -m_dims.y / 2, m_dims.z / 2));
	verts->addPoint(CCVector3(-m_dims.x / 2, m_dims.y / 2, m_dims.z / 2));
	verts->addPoint(CCVector3(m_dims.x / 2, m_dims.y / 2, m_dims.z / 2));
	verts->addPoint(CCVector3(m_dims.x / 2, -m_dims.y / 2, m_dims.z / 2));
	verts->addPoint(CCVector3(-m_dims.x / 2, -m_dims.y / 2, -m_dims.z / 2));
	verts->addPoint(CCVector3(-m_dims.x / 2, m_dims.y / 2, -m_dims.z / 2));
	verts->addPoint(CCVector3(m_dims.x / 2, m_dims.y / 2, -m_dims.z / 2));
	verts->addPoint(CCVector3(m_dims.x / 2, -m_dims.y / 2, -m_dims.z / 2));

	// front face (+Z): triangles (0,2,1) and (0,3,2)
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, 0, 1)));
	addTriangle(0, 2, 1);
	addTriangleNormalIndexes(0, 0, 0);
	addTriangle(0, 3, 2);
	addTriangleNormalIndexes(0, 0, 0);

	// left face (-X): triangles (4,1,5) and (4,0,1)
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(-1, 0, 0)));
	addTriangle(4, 1, 5);
	addTriangleNormalIndexes(1, 1, 1);
	addTriangle(4, 0, 1);
	addTriangleNormalIndexes(1, 1, 1);

	// back face (-Z): triangles (7,5,6) and (7,4,5)
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, 0, -1)));
	addTriangle(7, 5, 6);
	addTriangleNormalIndexes(2, 2, 2);
	addTriangle(7, 4, 5);
	addTriangleNormalIndexes(2, 2, 2);

	// right face (+X): triangles (3,6,2) and (3,7,6)
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(1, 0, 0)));
	addTriangle(3, 6, 2);
	addTriangleNormalIndexes(3, 3, 3);
	addTriangle(3, 7, 6);
	addTriangleNormalIndexes(3, 3, 3);

	// lower face (-Y): triangles (4,3,0) and (4,7,3)
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, -1, 0)));
	addTriangle(4, 3, 0);
	addTriangleNormalIndexes(4, 4, 4);
	addTriangle(4, 7, 3);
	addTriangleNormalIndexes(4, 4, 4);

	// upper face (+Y): triangles (1,6,5) and (1,2,6)
	m_triNormals->addElement(ccNormalVectors::GetNormIndex(CCVector3(0, 1, 0)));
	addTriangle(1, 6, 5);
	addTriangleNormalIndexes(5, 5, 5);
	addTriangle(1, 2, 6);
	addTriangleNormalIndexes(5, 5, 5);

	return true;
}

// ccBox::clone
/**
 * @brief Clone this box
 */
ccGenericPrimitive* ccBox::clone() const
{
	return finishCloneJob(new ccBox(m_dims, &m_transformation, getName()));
}

// ccBox::toFile_MeOnly
/**
 * @brief Serialize box to file
 *
 * Writes m_dims (x, y, z) as three doubles.
 * File version: 21+
 */
bool ccBox::toFile_MeOnly(QFile& out, short dataVersion) const
{
	assert(out.isOpen() && (out.openMode() & QIODevice::WriteOnly));
	if (dataVersion < 21)
	{
		assert(false);
		return false;
	}

	if (!ccGenericPrimitive::toFile_MeOnly(out, dataVersion))
		return false;

	QDataStream outStream(&out);
	outStream << m_dims.x;
	outStream << m_dims.y;
	outStream << m_dims.z;

	return true;
}

// ccBox::fromFile_MeOnly
/**
 * @brief Deserialize box from file
 *
 * Reads m_dims (x, y, z) as three doubles.
 * File version: 21+
 */
bool ccBox::fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap)
{
	if (!ccGenericPrimitive::fromFile_MeOnly(in, dataVersion, flags, oldToNewIDMap))
		return false;

	QDataStream inStream(&in);
	ccSerializationHelper::CoordsFromDataStream(inStream, flags, m_dims.u, 3);

	return true;
}

// ccBox::minimumFileVersion_MeOnly
/**
 * @brief Minimum file version required for this class
 */
short ccBox::minimumFileVersion_MeOnly() const
{
	return std::max(static_cast<short>(21), ccGenericPrimitive::minimumFileVersion_MeOnly());
}
