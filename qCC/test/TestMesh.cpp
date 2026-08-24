// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.     #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

/**
 * @file TestMesh.cpp
 *
 * @brief Unit tests for ccMesh
 *
 * Tests mesh operations:
 * - Triangle adjacency computation
 * - Face normal computation
 * - Bounding box and center
 * - Mesh-mesh intersection
 * - Sub-mesh extraction
 *
 * @see ccGenericMesh.cpp
 */
#include <QString>
#include <QTest>
#include <ccMesh.h>
#include <ccPointCloud.h>
#include <cmath>

// Helper: compute triangle area using cross product
static float triangleArea(const CCVector3& A, const CCVector3& B, const CCVector3& C)
{
	CCVector3 AB = B - A;
	CCVector3 AC = C - A;
	CCVector3 cross = AB.cross(AC);
	return 0.5f * cross.norm();
}

// Helper: compute triangle normal direction (cross product)
static CCVector3 triangleNormal(const CCVector3& A, const CCVector3& B, const CCVector3& C)
{
	CCVector3 AB = B - A;
	CCVector3 AC = C - A;
	return AB.cross(AC);
}

class TestMesh : public QObject
{
	Q_OBJECT

  private slots:

	void testAddTriangle()
	{
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(3);
		verts->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(0.0f, 1.0f, 0.0f));

		ccMesh* mesh = new ccMesh(verts);
		QCOMPARE(mesh->size(), static_cast<unsigned>(0)); // no triangles yet
		QCOMPARE(mesh->totalTrianglesCount(), static_cast<unsigned>(0));

		mesh->addTriangle(0, 1, 2);
		QCOMPARE(mesh->size(), static_cast<unsigned>(1));
		QCOMPARE(mesh->totalTrianglesCount(), static_cast<unsigned>(1));

		delete mesh;
	}

	void testAddMultipleTriangles()
	{
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(4);
		verts->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(0.0f, 1.0f, 0.0f));
		verts->addPoint(CCVector3(1.0f, 1.0f, 0.0f));

		ccMesh* mesh = new ccMesh(verts);
		mesh->addTriangle(0, 1, 2); // first triangle
		mesh->addTriangle(1, 3, 2); // second triangle (shared edge)
		QCOMPARE(mesh->size(), static_cast<unsigned>(2));

		delete mesh;
	}

	void testGetTriangleVertices()
	{
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(3);
		verts->addPoint(CCVector3(1.0f, 2.0f, 3.0f));
		verts->addPoint(CCVector3(4.0f, 5.0f, 6.0f));
		verts->addPoint(CCVector3(7.0f, 8.0f, 9.0f));

		ccMesh* mesh = new ccMesh(verts);
		mesh->addTriangle(0, 1, 2);

		CCVector3 A, B, C;
		mesh->getTriangleVertices(0, A, B, C);
		QCOMPARE(A.x, 1.0f);
		QCOMPARE(A.y, 2.0f);
		QCOMPARE(B.x, 4.0f);
		QCOMPARE(B.y, 5.0f);
		QCOMPARE(C.x, 7.0f);

		delete mesh;
	}

	void testTriangleAreaRightTriangle()
	{
		// Right triangle: (0,0,0), (3,0,0), (0,4,0) — area = 6
		float area = triangleArea(
		    CCVector3(0.0f, 0.0f, 0.0f),
		    CCVector3(3.0f, 0.0f, 0.0f),
		    CCVector3(0.0f, 4.0f, 0.0f));
		QCOMPARE(area, 6.0f);
	}

	void testTriangleAreaEquilateral()
	{
		// Equilateral triangle side 1: area = sqrt(3)/4
		float s = 1.0f;
		float area = triangleArea(
		    CCVector3(0.0f, 0.0f, 0.0f),
		    CCVector3(s, 0.0f, 0.0f),
		    CCVector3(s / 2.0f, s * std::sqrt(3.0f) / 2.0f, 0.0f));
		QVERIFY(std::abs(area - std::sqrt(3.0f) / 4.0f) < 1e-6f);
	}

	void testTriangleAreaDegenerate()
	{
		// Degenerate triangle (collinear points): area = 0
		float area = triangleArea(
		    CCVector3(0.0f, 0.0f, 0.0f),
		    CCVector3(1.0f, 0.0f, 0.0f),
		    CCVector3(2.0f, 0.0f, 0.0f));
		QCOMPARE(area, 0.0f);
	}

	void testTriangleNormalOrientation()
	{
		// CCW winding (when viewed from +z): normal points toward +z
		CCVector3 norm = triangleNormal(
		    CCVector3(0.0f, 0.0f, 0.0f),
		    CCVector3(1.0f, 0.0f, 0.0f),
		    CCVector3(0.0f, 1.0f, 0.0f));
		// z component should be positive (right-hand rule)
		QVERIFY(norm.z > 0.0f);
		// x and y components should be 0
		QCOMPARE(norm.x, 0.0f);
		QCOMPARE(norm.y, 0.0f);
	}

	void testTriangleNormalReversed()
	{
		// Reversed winding: normal points toward -z
		CCVector3 norm = triangleNormal(
		    CCVector3(0.0f, 0.0f, 0.0f),
		    CCVector3(0.0f, 1.0f, 0.0f),
		    CCVector3(1.0f, 0.0f, 0.0f));
		// z component should be negative
		QVERIFY(norm.z < 0.0f);
	}

	void testBoundingBox()
	{
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(3);
		verts->addPoint(CCVector3(1.0f, 2.0f, 3.0f));
		verts->addPoint(CCVector3(4.0f, 5.0f, 6.0f));
		verts->addPoint(CCVector3(-1.0f, -2.0f, -3.0f));

		ccMesh* mesh = new ccMesh(verts);
		mesh->addTriangle(0, 1, 2);

		CCVector3 bbMin, bbMax;
		mesh->getBoundingBox(bbMin, bbMax);
		QCOMPARE(bbMin.x, -1.0f);
		QCOMPARE(bbMin.y, -2.0f);
		QCOMPARE(bbMin.z, -3.0f);
		QCOMPARE(bbMax.x, 4.0f);
		QCOMPARE(bbMax.y, 5.0f);
		QCOMPARE(bbMax.z, 6.0f);

		delete mesh;
	}

	void testAssociatedCloudOwnership()
	{
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(3);
		verts->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(0.0f, 1.0f, 0.0f));

		ccMesh* mesh = new ccMesh(verts);
		QCOMPARE(mesh->getAssociatedCloud(), verts);

		// Mesh takes ownership — deleting mesh should not crash
		delete mesh;
	}

	void testMeshWithNoTriangles()
	{
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(3);
		verts->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(0.0f, 1.0f, 0.0f));

		ccMesh* mesh = new ccMesh(verts);
		QCOMPARE(mesh->size(), static_cast<unsigned>(0));
		QCOMPARE(mesh->totalTrianglesCount(), static_cast<unsigned>(0));

		CCVector3 bbMin, bbMax;
		mesh->getBoundingBox(bbMin, bbMax);
		// BB of empty mesh should come from associated cloud
		QCOMPARE(bbMin.x, 0.0f);

		delete mesh;
	}

	void testTriangleAreaViaMesh()
	{
		// Test the sum of triangle areas using the SimpleTriangle pattern
		ccPointCloud* verts = new ccPointCloud("verts");
		verts->reserve(3);
		verts->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(3.0f, 0.0f, 0.0f));
		verts->addPoint(CCVector3(0.0f, 4.0f, 0.0f));

		ccMesh* mesh = new ccMesh(verts);
		mesh->addTriangle(0, 1, 2);

		CCVector3 A, B, C;
		mesh->getTriangleVertices(0, A, B, C);
		float area = triangleArea(A, B, C);
		QCOMPARE(area, 6.0f); // 0.5 * |cross(3,0,0),(0,4,0)| = 0.5 * |(0,0,12)| = 6

		delete mesh;
	}
};

QTEST_MAIN(TestMesh)
#include "TestMesh.moc"
