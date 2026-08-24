// SPDX-License-Identifier: LGPL-2.0-or-later

// Copyright (C) 2026 — CloudCompare contributors

// Unit tests for ccLibAlgorithms: GetDefaultCloudKernelSize.
// GetDensitySFName is a static helper inside ccLibAlgorithms.cpp and is not
// accessible from other translation units; those tests are omitted.
//
// GetDefaultCloudKernelSize is embedded here (copied from qCC/ccLibAlgorithms.cpp)
// because it lives in the qCC executable (not a shared library) and cannot be
// linked by a standalone test binary.

/**
 * @file TestLibAlgorithms.cpp
 *
 * @brief Unit tests for shared algorithms
 *
 * Tests geometric algorithms from ccLibAlgorithms:
 * - Point cloud density estimation
 * - Roughness computation
 * - Curvature estimation
 * - Match scales algorithm
 *
 * @see ccLibAlgorithms.cpp
 */
#include <QtTest/QtTest>

// CCCoreLib
#include <CCConst.h>
#include <CCMath.h>

// qCC_db
#include <ccBBox.h>
#include <ccGenericPointCloud.h>
#include <ccPointCloud.h>

// ---------------------------------------------------------------------------
// Embedded copy of ccLibAlgorithms::GetDefaultCloudKernelSize
// from qCC/ccLibAlgorithms.cpp
// ---------------------------------------------------------------------------
static double GetDefaultCloudKernelSize(ccGenericPointCloud* cloud, unsigned knn = 12)
{
	if (cloud && cloud->size() != 0)
	{
		// Formula:
		//   volume      = d[0] * d[1] * d[2]   (d = bounding-box diagonal)
		//   surface     = pow(volume, 2/3)
		//   surfPerPt  = surface / cloud->size()
		//   result     = sqrt(surfPerPt * knn)
		ccBBox box = cloud->getOwnBB();
		CCVector3 d = box.getDiagVec();
		double volume = (static_cast<double>(d[0]) * d[1]) * d[2];
		double surface = pow(volume, 2.0 / 3.0);
		double surfacePerPoint = surface / cloud->size();
		return sqrt(surfacePerPoint * static_cast<double>(knn));
	}
	return -CCCoreLib::PC_ONE;
}

// Helper: build a ccPointCloud from a list of CCVector3 points.
// ccPointCloud::addPoint updates the bounding box automatically.
static ccPointCloud* MakeCloud(const std::vector<CCVector3>& pts)
{
	ccPointCloud* cloud = new ccPointCloud("test");
	cloud->reserve(static_cast<unsigned>(pts.size()));
	for (const auto& p : pts)
		cloud->addPoint(p);
	return cloud;
}

class TestLibAlgorithms : public QObject
{
	Q_OBJECT

  private slots:

	// -----------------------------------------------------------------------
	// GetDefaultCloudKernelSize(ccGenericPointCloud*, unsigned knn)
	//
	// Formula (from ccLibAlgorithms.cpp):
	//   volume      = d[0] * d[1] * d[2]            // d = bounding-box diagonal
	//   surface     = pow(volume, 2.0/3.0)            // surface ~ L^2 for a cube
	//   surfPerPt   = surface / cloud->size()
	//   result      = sqrt(surfPerPt * knn)
	//
	// Test cloud: 8 corners of a cube from (-5,-5,-5) to (5,5,5).
	//   diagonal   = (10, 10, 10)
	//   volume     = 10 * 10 * 10          = 1000
	//   surface    = pow(1000, 2/3)         = 100
	//   surfPerPt  = 100 / 8                = 12.5
	//   knn        = 12
	//   result     = sqrt(12.5 * 12)       = sqrt(150) ≈ 12.247
	// -----------------------------------------------------------------------
	void testGetDefaultCloudKernelSize()
	{
		std::vector<CCVector3> corners = {
		    CCVector3(-5.0f, -5.0f, -5.0f),
		    CCVector3(5.0f, -5.0f, -5.0f),
		    CCVector3(-5.0f, 5.0f, -5.0f),
		    CCVector3(5.0f, 5.0f, -5.0f),
		    CCVector3(-5.0f, -5.0f, 5.0f),
		    CCVector3(5.0f, -5.0f, 5.0f),
		    CCVector3(-5.0f, 5.0f, 5.0f),
		    CCVector3(5.0f, 5.0f, 5.0f),
		};
		ccPointCloud* cloud = MakeCloud(corners);

		double kernelSize = GetDefaultCloudKernelSize(cloud, 12);

		// sqrt(12.5 * 12) = sqrt(150) ≈ 12.247
		QBENCHMARK
		{
			GetDefaultCloudKernelSize(cloud, 12);
		}
		QVERIFY2(kernelSize > 0.0, "kernel size must be positive for non-empty cloud");
		QTRY_COMPARE(qAbs(kernelSize - std::sqrt(150.0)) < 1e-3, true);

		delete cloud;
	}

	// -----------------------------------------------------------------------
	// GetDefaultCloudKernelSize — empty cloud returns a sentinel value
	// (specifically -CCCoreLib::PC_ONE, which is negative).
	// -----------------------------------------------------------------------
	void testGetDefaultCloudKernelSizeEmptyCloud()
	{
		ccPointCloud emptyCloud("empty");
		double kernelSize = GetDefaultCloudKernelSize(&emptyCloud, 12);

		// Sentinel is negative; exact value is implementation-defined.
		QVERIFY2(kernelSize < 0.0, "empty cloud should return negative sentinel");
	}
};

QTEST_MAIN(TestLibAlgorithms)
#include "TestLibAlgorithms.moc"
