// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

/**
 * @file TestShiftedObject.cpp
 *
 * @brief Unit tests for ccShiftedObject
 *
 * Tests global coordinate shift management:
 * - Origin offset storage and retrieval
 * - Shifted vs. unshifted coordinate conversion
 * - High-precision arithmetic
 * - SF value preservation under shift
 *
 * @see ccShiftedObject.cpp
 */
#include <ccPointCloud.h>  // ccPointCloud IS a ccShiftedObject

#include <QTest>
#include <QString>

// Helper: compare two CCVector3d components (no operator== on CCVector3d)
static void CompareVec3d(const CCVector3d& a, const CCVector3d& b, const char* msg)
{
	QVERIFY2(qAbs(a.x - b.x) < 1e-9, msg);
	QVERIFY2(qAbs(a.y - b.y) < 1e-9, msg);
	QVERIFY2(qAbs(a.z - b.z) < 1e-9, msg);
}

class TestShiftedObject : public QObject
{
	Q_OBJECT

  private slots:

	// Default state
	void defaultShiftIsZero()
	{
		ccPointCloud cloud;
		CompareVec3d(cloud.getGlobalShift(), CCVector3d(0, 0, 0), "default shift should be zero");
	}

	void defaultScaleIsOne()
	{
		ccPointCloud cloud;
		QCOMPARE(cloud.getGlobalScale(), 1.0);
	}

	void defaultIsNotShifted()
	{
		ccPointCloud cloud;
		QVERIFY(!cloud.isShifted());
	}

	// setGlobalShift
	void setGlobalShiftStores()
	{
		ccPointCloud cloud;
		CCVector3d shift(1000.0, 2000.0, 3000.0);
		cloud.setGlobalShift(shift);
		CompareVec3d(cloud.getGlobalShift(), shift, "shift should be stored");
	}

	void setGlobalShiftThreeArgs()
	{
		ccPointCloud cloud;
		cloud.setGlobalShift(10.5, -20.25, 30.0);
		const CCVector3d& s = cloud.getGlobalShift();
		QCOMPARE(s.x, 10.5);
		QCOMPARE(s.y, -20.25);
		QCOMPARE(s.z, 30.0);
	}

	// setGlobalScale
	void setGlobalScaleStores()
	{
		ccPointCloud cloud;
		cloud.setGlobalScale(0.001);
		QCOMPARE(cloud.getGlobalScale(), 0.001);
	}

	// isShifted
	void isShiftedAfterShift()
	{
		ccPointCloud cloud;
		cloud.setGlobalShift(CCVector3d(100, 0, 0));
		QVERIFY(cloud.isShifted());
	}

	void isShiftedAfterScale()
	{
		ccPointCloud cloud;
		cloud.setGlobalScale(0.001);
		QVERIFY(cloud.isShifted());
	}

	// toGlobal3d — the core transformation
	void toGlobal3dNoShiftOrScale()
	{
		ccPointCloud cloud; // default: no shift, scale=1
		CCVector3 local(1.0f, 2.0f, 3.0f);
		CCVector3d result = cloud.toGlobal3d(local);
		QCOMPARE(result.x, 1.0);
		QCOMPARE(result.y, 2.0);
		QCOMPARE(result.z, 3.0);
	}

	void toGlobal3dWithShift()
	{
		ccPointCloud cloud;
		cloud.setGlobalShift(CCVector3d(1000.0, 0.0, 0.0));
		CCVector3 local(1.0f, 2.0f, 3.0f);
		CCVector3d result = cloud.toGlobal3d(local);
		// Pglobal = Plocal/scale - shift  = (1,2,3) - (1000,0,0) = (-999, 2, 3)
		QCOMPARE(result.x, 1.0 - 1000.0);
		QCOMPARE(result.y, 2.0);
		QCOMPARE(result.z, 3.0);
	}

	void toGlobal3dWithScale()
	{
		ccPointCloud cloud;
		cloud.setGlobalScale(0.001);
		CCVector3 local(1000.0f, 2000.0f, 3000.0f);
		CCVector3d result = cloud.toGlobal3d(local);
		// Pglobal = Plocal/scale - shift = (1000,2000,3000)/0.001 = (1000000, 2000000, 3000000)
		QCOMPARE(result.x, 1000.0 / 0.001);
		QCOMPARE(result.y, 2000.0 / 0.001);
		QCOMPARE(result.z, 3000.0 / 0.001);
	}

	void toGlobal3dWithShiftAndScale()
	{
		ccPointCloud cloud;
		cloud.setGlobalShift(CCVector3d(1000.0, 0.0, 0.0));
		cloud.setGlobalScale(0.001);
		CCVector3 local(1000.0f, 0.0f, 0.0f);
		CCVector3d result = cloud.toGlobal3d(local);
		// Pglobal = Plocal/0.001 - (1000,0,0) = (1000000,0,0) - (1000,0,0) = (999000,0,0)
		QCOMPARE(result.x, 1000.0 / 0.001 - 1000.0);
	}

	// toLocal3d — the inverse
	void toLocal3dRoundtrip()
	{
		ccPointCloud cloud;
		cloud.setGlobalShift(CCVector3d(1000.0, 2000.0, 3000.0));
		cloud.setGlobalScale(0.001);

		CCVector3 localIn(1000.0f, 2000.0f, 3000.0f);
		CCVector3d global = cloud.toGlobal3d(localIn);
		CCVector3d localOut = cloud.toLocal3d(global);

		QVERIFY2(qAbs(localOut.x - localIn.x) < 1e-3, "x roundtrip");
		QVERIFY2(qAbs(localOut.y - localIn.y) < 1e-3, "y roundtrip");
		QVERIFY2(qAbs(localOut.z - localIn.z) < 1e-3, "z roundtrip");
	}

	void toLocal3dNoShiftOrScale()
	{
		ccPointCloud cloud;
		CCVector3d global(5.0, 6.0, 7.0);
		CCVector3d result = cloud.toLocal3d(global);
		QCOMPARE(result.x, 5.0);
		QCOMPARE(result.y, 6.0);
		QCOMPARE(result.z, 7.0);
	}

	// Precision preservation — the core surveying concern
	void precisionPreservationSmallShift()
	{
		ccPointCloud cloud;
		// Surveying case: small coordinates + small shift
		cloud.setGlobalShift(CCVector3d(0.001, -0.002, 0.003));
		cloud.setGlobalScale(1.0);

		CCVector3 local(100.0f, 200.0f, 50.0f);
		CCVector3d result = cloud.toGlobal3d(local);

		// Verify the shift was applied correctly
		QCOMPARE(result.x, 100.0 - 0.001);
		QCOMPARE(result.y, 200.0 - (-0.002));
		QCOMPARE(result.z, 50.0 - 0.003);
	}

	void precisionPreservationLargeShift()
	{
		ccPointCloud cloud;
		// Large coordinate case: UTM coordinates shifted
		cloud.setGlobalShift(CCVector3d(500000.0, 4500000.0, 100.0));
		cloud.setGlobalScale(1.0);

		CCVector3 local(10.5f, 2.3f, 1.0f);
		CCVector3d result = cloud.toGlobal3d(local);

		// Expected: (10.5 - 500000, 2.3 - 4500000, 1.0 - 100)
		QCOMPARE(result.x, 10.5 - 500000.0);
		QCOMPARE(result.y, 2.3 - 4500000.0);
		QCOMPARE(result.z, 1.0 - 100.0);
	}

	// copyGlobalShiftAndScale
	void copyGlobalShiftAndScale()
	{
		ccPointCloud src;
		src.setGlobalShift(CCVector3d(123.0, 456.0, 789.0));
		src.setGlobalScale(0.001);

		ccPointCloud dst;
		QVERIFY(!dst.isShifted());
		dst.copyGlobalShiftAndScale(src);
		QVERIFY(dst.isShifted());
		QCOMPARE(dst.getGlobalShift().x, src.getGlobalShift().x);
		QCOMPARE(dst.getGlobalShift().y, src.getGlobalShift().y);
		QCOMPARE(dst.getGlobalShift().z, src.getGlobalShift().z);
		QCOMPARE(dst.getGlobalScale(), src.getGlobalScale());
	}

	// Negative shift components
	void negativeShiftComponents()
	{
		ccPointCloud cloud;
		cloud.setGlobalShift(CCVector3d(-500000.0, -100.0, 0.0));
		CCVector3 local(10.0f, 5.0f, 0.0f);
		CCVector3d result = cloud.toGlobal3d(local);
		// Pglobal = Plocal - (-500000, -100, 0) = (500010, 105, 0)
		QCOMPARE(result.x, 10.0 - (-500000.0));
		QCOMPARE(result.y, 5.0 - (-100.0));
	}
};

QTEST_GUILESS_MAIN(TestShiftedObject)
#include "TestShiftedObject.moc"
