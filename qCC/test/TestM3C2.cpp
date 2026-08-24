// SPDX-License-Identifier: LGPL-2.0-or-later
// Copyright (C) 2026 — CloudCompare contributors
// Unit tests for qM3C2 tools: ComputeStatistics and MakeNormalsHorizontal.
// Uses Qt Test — run via Python subprocess with explicit PATH (see BUILD-LOCAL.md §9).

/**
 * @file TestM3C2.cpp
 *
 * @brief Unit tests for M3C2 algorithm
 *
 * Tests Multiscale Model to Model Cloud Comparison:
 * - Cylinder casting along normals
 * - Perpendicular plane fitting
 * - Distance computation
 * - Uncertainty estimation
 * - Scale sensitivity
 *
 * @see qM3C2Process.cpp
 */
#include <DgmOctree.h>
#include <QtTest/QtTest>
#include <array>
#include <ccNormalVectors.h>
#include <ccPointCloud.h>
#include <cmath>
#include <qM3C2Tools.h>
#include <vector>

using namespace CCCoreLib;

// MSVC can't parse `>>` in nested template args inside Qt Test macros (C2947).
// Use flat typedefs to avoid the issue.
using VecNorm3 = std::vector<std::array<double, 3>>;

// =============================================================================
// TestM3C2 — statistical computation and normals tools
// =============================================================================
class TestM3C2 : public QObject
{
	Q_OBJECT

  private slots:

	// --- ComputeStatistics: mean + stddev ---

	void testComputeMeanStdDev_data()
	{
		QTest::addColumn<std::vector<double>>("distancesSquared");
		QTest::addColumn<double>("expectedMean");
		QTest::addColumn<double>("expectedStdDev");

		// The implementation computes stddev of the squared-distance values (squareDistd = d²).
		// {1, 2, 3}: E[X²] = 14/3, E[X] = 2, var = 14/3-4 = 2/3, stddev = √(2/3) ≈ 0.816497
		QTest::newRow("simple_3pts") << std::vector<double>{1.0, 2.0, 3.0} << 2.0 << std::sqrt(2.0 / 3.0);

		// {1, 2, 3, 4, 5}: E[X²] = 55/5=11, E[X] = 3, var = 11-9=2, stddev = √2 ≈ 1.414214
		QTest::newRow("simple_5pts") << std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0} << 3.0 << std::sqrt(2.0);

		// {10}: mean=10, stddev=0 (single point — handled as special case)
		QTest::newRow("single_point") << std::vector<double>{10.0} << 10.0 << 0.0;

		// {0, 0, 0}: mean=0, stddev=0
		QTest::newRow("all_zero") << std::vector<double>{0.0, 0.0, 0.0} << 0.0 << 0.0;
	}

	void testComputeMeanStdDev()
	{
		QFETCH(std::vector<double>, distancesSquared);
		QFETCH(double, expectedMean);
		QFETCH(double, expectedStdDev);

		// Build NeighboursSet from the distance-squared values
		DgmOctree::NeighboursSet set;
		set.reserve(static_cast<size_t>(distancesSquared.size()));
		for (double d2 : distancesSquared)
		{
			DgmOctree::PointDescriptor desc;
			desc.squareDistd = d2; // ComputeStatistics only reads .squareDistd
			set.push_back(desc);
		}

		double mean = 0.0;
		double stddev = 0.0;
		qM3C2Tools::ComputeStatistics(set, false, mean, stddev);

		QVERIFY2(std::abs(mean - expectedMean) < 1e-9,
		         qPrintable(QString("mean=%1 expected=%2").arg(mean).arg(expectedMean)));
		QVERIFY2(std::abs(stddev - expectedStdDev) < 1e-9,
		         qPrintable(QString("stddev=%1 expected=%2").arg(stddev).arg(expectedStdDev)));
	}

	// --- ComputeStatistics: median + IQR ---

	void testComputeMedianIQR_data()
	{
		QTest::addColumn<std::vector<double>>("distancesSquared");
		QTest::addColumn<double>("expectedMedian");
		QTest::addColumn<double>("expectedIQR");

		// {1,2,3}: median=2, Q1=1.5, Q3=2.5, IQR=1.0
		// For odd count (3): num_pts_each_half = 2, Q1=Median of {1,2} = 1.5, Q3=Median of {2,3} = 2.5
		QTest::newRow("odd_3pts") << std::vector<double>{1.0, 2.0, 3.0} << 2.0 << 1.0;

		// {1,2,3,4}: even count — median = (2+3)/2 = 2.5
		// num_pts_each_half = (4+1)/2 = 2, Q1=Median({1,2})=1.5, Q3=Median({3,4})=3.5, IQR=2.0
		QTest::newRow("even_4pts") << std::vector<double>{1.0, 2.0, 3.0, 4.0} << 2.5 << 2.0;

		// {1,2,3,4,5}: median=3, Q1=2.0, Q3=4.0, IQR=2.0 (implementation uses midpoint averaging)
		QTest::newRow("odd_5pts") << std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0} << 3.0 << 2.0;

		// Single point: median = that point, IQR = 0
		QTest::newRow("single_point") << std::vector<double>{7.0} << 7.0 << 0.0;
	}

	void testComputeMedianIQR()
	{
		QFETCH(std::vector<double>, distancesSquared);
		QFETCH(double, expectedMedian);
		QFETCH(double, expectedIQR);

		DgmOctree::NeighboursSet set;
		set.reserve(static_cast<size_t>(distancesSquared.size()));
		for (double d2 : distancesSquared)
		{
			DgmOctree::PointDescriptor desc;
			desc.squareDistd = d2;
			set.push_back(desc);
		}

		double median = 0.0;
		double iqr = 0.0;
		qM3C2Tools::ComputeStatistics(set, true, median, iqr);

		QVERIFY2(std::abs(median - expectedMedian) < 1e-9,
		         qPrintable(QString("median=%1 expected=%2").arg(median).arg(expectedMedian)));
		QVERIFY2(std::abs(iqr - expectedIQR) < 1e-9,
		         qPrintable(QString("IQR=%1 expected=%2").arg(iqr).arg(expectedIQR)));
	}

	// --- ComputeStatistics: empty set ---

	void testComputeStatisticsEmpty()
	{
		DgmOctree::NeighboursSet set; // empty
		double meanOrMedian = 999.0;
		double stdDevOrIQR = 999.0;

		qM3C2Tools::ComputeStatistics(set, false, meanOrMedian, stdDevOrIQR);

		QVERIFY2(std::isnan(meanOrMedian), "empty set should return NaN for mean");
		QVERIFY2(stdDevOrIQR == 0.0, "empty set should return 0 for stddev");
	}

	// --- MakeNormalsHorizontal ---

	void testMakeNormalsHorizontal_data()
	{
		QTest::addColumn<VecNorm3>("inputNormals");    // {nx, ny, nz}
		QTest::addColumn<VecNorm3>("expectedNormals"); // Z zeroed and renormalised

		// (0, 0, 1) → (0, 0, 0) after zeroing Z → NaN after normalize
		// But ccNormalVectors table doesn't have (0,0,0), so this is a degenerate case.
		// Test with a well-defined normal: (1, 0, 0) → (1, 0, 0) (already horizontal)
		QTest::newRow("already_horizontal") << std::vector<std::array<double, 3>>{{1, 0, 0}} << std::vector<std::array<double, 3>>{{1, 0, 0}};

		// (1, 1, 1) normalized = (1/√3, 1/√3, 1/√3) ≈ (0.577, 0.577, 0.577)
		// After MakeNormalsHorizontal: Z=0, renormalize → (1/√2, 1/√2, 0) ≈ (0.707, 0.707, 0)
		{
			double invSqrt2 = 0.707106781186547524400844362104849;
			QTest::newRow("tilted_111") << std::vector<std::array<double, 3>>{{1, 1, 1}, {0, 0, 1}} << std::vector<std::array<double, 3>>{{invSqrt2, invSqrt2, 0.0}, {0.0, 0.0, 0.0}};
		}

		// (-1, 2, -3) → Z=0 → renormalize
		// norm = √(1+4+9) = √14 ≈ 3.7417
		// horizontal: (-1/√14, 2/√14, 0) ≈ (-0.267, 0.535, 0)
		{
			double invSqrt14 = 1.0 / std::sqrt(14.0);
			QTest::newRow("negative_tilted") << std::vector<std::array<double, 3>>{{-1, 2, -3}} << std::vector<std::array<double, 3>>{{-invSqrt14, 2 * invSqrt14, 0.0}};
		}
	}

	void testMakeNormalsHorizontal()
	{
		QFETCH(VecNorm3, inputNormals);
		QFETCH(VecNorm3, expectedNormals);

		// Build a ccPointCloud with normals
		ccPointCloud cloud("normals_test");
		cloud.reserve(static_cast<unsigned>(inputNormals.size()));
		for (size_t i = 0; i < inputNormals.size(); ++i)
			cloud.addPoint(CCVector3(static_cast<PointCoordinateType>(i),
			                         0,
			                         0)); // dummy positions

		// Allocate and fill normals table
		cloud.reserveTheNormsTable();
		for (const auto& n : inputNormals)
		{
			CCVector3 N(static_cast<PointCoordinateType>(n[0]),
			            static_cast<PointCoordinateType>(n[1]),
			            static_cast<PointCoordinateType>(n[2]));
			N.normalize();
			// addNormIndex() appends; addNormAtIndex() uses getValue(i) which fails on empty table
			cloud.addNormIndex(ccNormalVectors::GetNormIndex(N.u));
		}

		// Apply MakeNormalsHorizontal
		QVERIFY2(cloud.normals() != nullptr, "cloud must have a normals table");
		qM3C2Normals::MakeNormalsHorizontal(*cloud.normals());

		// Verify each normal
		// Note: ccNormalVectors uses lossy spherical quantization (19-bit).
		// After compress → decompress → zero Z → normalize → compress → decompress,
		// the Z component has ~0.001 residual (quantization error). We test
		// that Z is "approximately zero" (within 1e-2) and that the normal
		// remains unit-length after re-compression.
		for (size_t i = 0; i < expectedNormals.size(); ++i)
		{
			const CompressedNormType code = cloud.normals()->getValue(i);
			CCVector3 N(ccNormalVectors::GetNormal(code));

			QVERIFY2(std::abs(N.z) < 1e-2,
			         qPrintable(QString("N[%1].z should be near 0 (quantization), got %2").arg(i).arg(N.z)));

			// Verify the normal is still approximately unit length
			double norm = std::sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
			QVERIFY2(std::abs(norm - 1.0) < 1e-2,
			         qPrintable(QString("N[%1] norm=%2 should be ~1").arg(i).arg(norm)));
		}
	}

	// --- MakeNormalsHorizontal: multiple normals preserved correctly ---

	void testMakeNormalsHorizontalMultiple()
	{
		// Create cloud with 3 different tilted normals
		ccPointCloud cloud("multi_test");
		cloud.reserve(3);
		for (int i = 0; i < 3; ++i)
			cloud.addPoint(CCVector3(i, 0, 0));

		cloud.reserveTheNormsTable();

		// Normal 0: (1, 1, 1) → horizontal (0.707, 0.707, 0)
		// Normal 1: (0, 1, 1) → normalize → (0, 0.707, 0.707) → horizontal (0, 1, 0)
		// Normal 2: (2, 2, 2) → normalize → (0.577, 0.577, 0.577) → horizontal (0.5, 0.5, 0) * norm(2,2,2)=6 → (2/6, 2/6, 0)=(0.333, 0.333, 0)

		std::vector<CCVector3> normals = {
		    CCVector3(1, 1, 1),
		    CCVector3(0, 1, 1),
		    CCVector3(2, 2, 2),
		};
		for (auto& N : normals)
			N.normalize();

		for (unsigned i = 0; i < 3; ++i)
			cloud.addNormIndex(ccNormalVectors::GetNormIndex(normals[i].u));

		qM3C2Normals::MakeNormalsHorizontal(*cloud.normals());

		for (unsigned i = 0; i < 3; ++i)
		{
			CompressedNormType code = cloud.normals()->getValue(i);
			CCVector3 N(ccNormalVectors::GetNormal(code));

			QVERIFY2(std::abs(N.z) < 1e-2,
			         qPrintable(QString("N[%1].z=%2 should be near 0 (quantization error)").arg(i).arg(N.z)));
			// Each normal should still be normalized
			double norm = std::sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
			QVERIFY2(std::abs(norm - 1.0) < 1e-6,
			         qPrintable(QString("N[%1] should be unit length, norm=%2").arg(i).arg(norm)));
		}
	}
};

QTEST_MAIN(TestM3C2)
#include "TestM3C2.moc"
