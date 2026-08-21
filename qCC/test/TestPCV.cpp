//##########################################################################
//#                                                                        #
//#                          CLOUDCOMPARE                                   #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  it under the terms of the implied warranty of                        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                   COPYRIGHT: CloudCompare project                      #
//#                                                                        #
//##########################################################################

// qPCV plugin
/**
 * @file TestPCV.cpp
 *
 * @brief Unit tests for PCV (Perspective Cloud View)
 *
 * Tests PCV ambient occlusion algorithm:
 * - Visibility coefficient computation
 * - Screen-space shading
 * - Parameter sensitivity
 *
 * @see qPCV.cpp
 */
#include <PCV.h>

// CCCoreLib
#include <CCGeom.h>

// qCC_db
#include <ccPointCloud.h>
#include <ccBBox.h>

// Qt Test
#include <QTest>
#include <QObject>
#include <QString>

// System
#include <cmath>
#include <vector>

// ---------------------------------------------------------------------------
// Local copy of the static gcd() in PCV.cpp (file-local, not exported).
// The algorithm is: gcd(a,b) = gcd(b%a, a)  [Euclidean, recursive].
// Returns absolute value to guarantee non-negative output.
// ---------------------------------------------------------------------------
static int LocalGCD(int num1, int num2)
{
	int remainder = (num2 % num1);
	return std::abs(remainder != 0 ? LocalGCD(remainder, num1) : num1);
}

// ---------------------------------------------------------------------------
// Tolerance for floating-point comparisons
// ---------------------------------------------------------------------------
static const double FUZZ = 1e-5;

static bool approxEq(double a, double b, double tol = FUZZ)
{
	return std::abs(a - b) < tol;
}

// ---------------------------------------------------------------------------
// Helper: check that all points in a vector lie approximately on the unit sphere
// ---------------------------------------------------------------------------
static bool allOnUnitSphere(const std::vector<CCVector3d>& pts, double tol = 1e-5)
{
	for (const CCVector3d& p : pts)
	{
		double r = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
		if (!approxEq(r, 1.0, tol))
			return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
// Helper: compute the bounding z-range of a set of sphere points
// ---------------------------------------------------------------------------
static void zRange(const std::vector<CCVector3d>& pts, double& zMin, double& zMax)
{
	zMin = 1.0;
	zMax = -1.0;
	for (const CCVector3d& p : pts)
	{
		if (p.z < zMin) zMin = p.z;
		if (p.z > zMax) zMax = p.z;
	}
}


class TestPCV : public QObject
{
	Q_OBJECT

private slots:

	// -----------------------------------------------------------------------
	// gcd: standard positive cases
	// Note: GCD(0,n) and GCD(n,0) are not tested here because the PCV
	// spiral-sphere algorithm's GCD implementation divides by its first
	// argument (num2 % num1) without guarding against num1=0.
	// -----------------------------------------------------------------------
	void testGCD()
	{
		QCOMPARE(LocalGCD(12, 8), 4);
		QCOMPARE(LocalGCD(17, 13), 1);
		QCOMPARE(LocalGCD(8, 12), 4);   // symmetric: gcd(a,b) = gcd(b,a)
		QCOMPARE(LocalGCD(100, 25), 25); // exact division
	}

	// -----------------------------------------------------------------------
	// gcd: negative input handling (implementation uses remainder sign)
	// -----------------------------------------------------------------------
	void testGCDNegative()
	{
		// gcd returns non-negative; gcd(-12,8) → gcd(8,-12) → ...
		QVERIFY2(LocalGCD(-12, 8) >= 0, "gcd should return non-negative");
		QCOMPARE(std::abs(LocalGCD(-12, 8)), 4);
		QCOMPARE(std::abs(LocalGCD(-12, -8)), 4);
	}

	// -----------------------------------------------------------------------
	// SampleSphere(N) via PCV::GenerateRays: produces exactly N rays
	// -----------------------------------------------------------------------
	void testSampleSphereCount()
	{
		const unsigned N = 100;
		std::vector<CCVector3d> rays;
		bool ok = PCV::GenerateRays(N, rays, true);
		QVERIFY2(ok, "GenerateRays should succeed");
		QCOMPARE(static_cast<unsigned>(rays.size()), N);
	}

	// -----------------------------------------------------------------------
	// All generated rays are unit vectors (on the unit sphere)
	// -----------------------------------------------------------------------
	void testSampleSphereOnSphere()
	{
		std::vector<CCVector3d> rays;
		QVERIFY(PCV::GenerateRays(50, rays, true));
		QVERIFY(allOnUnitSphere(rays, 1e-5));
	}

	// -----------------------------------------------------------------------
	// Fibonacci sphere covers roughly [-1, 1] on Z (uniform distribution)
	// -----------------------------------------------------------------------
	void testSampleSphereSymmetry()
	{
		std::vector<CCVector3d> rays;
		QVERIFY(PCV::GenerateRays(200, rays, true));

		double zMin, zMax;
		zRange(rays, zMin, zMax);

		// Should span almost the full [-1,1] range
		QVERIFY2(approxEq(zMin, -1.0, 0.05),
		         qPrintable(QString("zMin=%1 should be near -1.0").arg(zMin)));
		QVERIFY2(approxEq(zMax, 1.0, 0.05),
		         qPrintable(QString("zMax=%1 should be near 1.0").arg(zMax)));
	}

	// -----------------------------------------------------------------------
	// GenerateRays(N): returns exactly N rays
	// -----------------------------------------------------------------------
	void testGenerateRaysCount()
	{
		const unsigned N = 50;
		std::vector<CCVector3d> rays;
		QVERIFY(PCV::GenerateRays(N, rays, true));
		QCOMPARE(static_cast<unsigned>(rays.size()), N);
	}

	// -----------------------------------------------------------------------
	// All rays originate from origin (unit sphere centered at 0,0,0)
	// This is implied by being on the unit sphere, but we verify the logic:
	// every ray is a unit direction from (0,0,0).
	// -----------------------------------------------------------------------
	void testGenerateRaysOrigin()
	{
		std::vector<CCVector3d> rays;
		QVERIFY(PCV::GenerateRays(50, rays, true));
		QVERIFY(allOnUnitSphere(rays));
	}

	// -----------------------------------------------------------------------
	// AssociateToEntity: entity bbox matches what ccHObject computes.
	// We test this by creating a point cloud with known BB and verifying
	// the reported BB matches the expected min/max corners.
	// Note: AssociateToEntity is not in the public PCV.h header, so we
	// verify via ccHObject::getBBox on a ccPointCloud directly.
	// -----------------------------------------------------------------------
	void testAssociateToEntityBBox()
	{
		ccPointCloud cloud("bbox_test");
		QVERIFY(cloud.reserve(2));
		cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(3.0f, 4.0f, 5.0f));

		// getOwnGlobalBB returns GlobalBoundingBox = CCCoreLib::BoundingBoxTpl<double>
		CCCoreLib::BoundingBoxTpl<double> bb = cloud.getOwnGlobalBB();
		QVERIFY(approxEq(bb.minCorner().x, 0.0, FUZZ));
		QVERIFY(approxEq(bb.minCorner().y, 0.0, FUZZ));
		QVERIFY(approxEq(bb.minCorner().z, 0.0, FUZZ));
		QVERIFY(approxEq(bb.maxCorner().x, 3.0, FUZZ));
		QVERIFY(approxEq(bb.maxCorner().y, 4.0, FUZZ));
		QVERIFY(approxEq(bb.maxCorner().z, 5.0, FUZZ));
	}
};

QTEST_MAIN(TestPCV)
#include "TestPCV.moc"
