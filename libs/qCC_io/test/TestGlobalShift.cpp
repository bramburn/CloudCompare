// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#include "ccGlobalShiftManager.h"
#include "ccPointCloud.h"

#include <QtTest/QtTest>

class TestGlobalShift : public QObject
{
	Q_OBJECT

  private slots:

	void testNeedShiftBelow()
	{
		// All coords well below MAX_COORDINATE_ABS_VALUE → no shift needed
		CCVector3d P(10.0, 20.0, 5.0);
		QVERIFY(!ccGlobalShiftManager::NeedShift(P));
	}

	void testNeedShiftAbove()
	{
		// At least one coord exceeds MAX_COORDINATE_ABS_VALUE → shift needed
		double originalMax = ccGlobalShiftManager::MaxCoordinateAbsValue();
		ccGlobalShiftManager::SetMaxCoordinateAbsValue(1e6); // 1 000 000

		CCVector3d P(1.5e6, 20.0, 5.0); // x exceeds threshold
		QVERIFY(ccGlobalShiftManager::NeedShift(P));

		ccGlobalShiftManager::SetMaxCoordinateAbsValue(originalMax);
	}

	void testBestShift()
	{
		// BestShift(CCVector3d) — each component is rounded to nearest power-of-10^3
		// if |component| >= MAX_COORDINATE_ABS_VALUE (1e6); otherwise returns 0.
		// roundOffScalePower starts at 3 (scale = 1000), so values round to multiples of 1000.

		// Below threshold → no shift for that component
		CCVector3d below = CCVector3d(999.0, 0.0, 0.0);
		CCVector3d resultBelow = ccGlobalShiftManager::BestShift(below);
		QCOMPARE(resultBelow.x, 0.0);
		QCOMPARE(resultBelow.y, 0.0);
		QCOMPARE(resultBelow.z, 0.0);

		// At boundary (exactly 1e6) → rounds to -1000000 (strict > comparison)
		CCVector3d boundary = CCVector3d(1e6, 0.0, 0.0);
		CCVector3d resultBoundary = ccGlobalShiftManager::BestShift(boundary);
		QCOMPARE(resultBoundary.x, -1000000.0);

		// Slightly above boundary → rounds to nearest 1000 multiple
		CCVector3d above = CCVector3d(1422671.0, 0.0, 0.0);
		CCVector3d resultAbove = ccGlobalShiftManager::BestShift(above);
		QCOMPARE(resultAbove.x, -1000000.0);  // 1422671 → int(1.422671) * 1000 = 1000*1000

		// Negative value → sign preserved
		CCVector3d negative = CCVector3d(-626146.0, 0.0, 0.0);
		CCVector3d resultNeg = ccGlobalShiftManager::BestShift(negative);
		QCOMPARE(resultNeg.x, 0.0);  // 626146 < 1e6 → below threshold

		// Very large negative → rounds with sign
		CCVector3d bigNeg = CCVector3d(-2200000.0, 0.0, 0.0);
		CCVector3d resultBigNeg = ccGlobalShiftManager::BestShift(bigNeg);
		QCOMPARE(resultBigNeg.x, -2000000.0);  // int(-2200) * 1000 = -2000*1000

		// Zero → no shift
		CCVector3d zero = CCVector3d(0.0, 0.0, 0.0);
		CCVector3d resultZero = ccGlobalShiftManager::BestShift(zero);
		QCOMPARE(resultZero.x, 0.0);
		QCOMPARE(resultZero.y, 0.0);
		QCOMPARE(resultZero.z, 0.0);

		// Multiple components: only x and z exceed threshold
		CCVector3d multi = CCVector3d(1.5e6, 500.0, -2.3e6);
		CCVector3d resultMulti = ccGlobalShiftManager::BestShift(multi);
		QCOMPARE(resultMulti.x, -1000000.0);   // 1500000 / 1000 = 1500 → 1500*1000
		QCOMPARE(resultMulti.y, 0.0);          // 500 < 1e6
		QCOMPARE(resultMulti.z, -2000000.0);   // -2300*1000
	}

	void testBestScale()
	{
		// scale = 10^(floor(log10(|d|)) - 6)
		// 1422671.72 → log10 ≈ 6.15 → floor = 6 → 10^(6-6) = 1.0
		QCOMPARE(ccGlobalShiftManager::BestScale(1422671.7232666016), 1.0);

		// 4.18e6 → log10 ≈ 6.62 → floor = 6 → 10^0 = 1.0
		QCOMPARE(ccGlobalShiftManager::BestScale(4188903.4295959473), 1.0);

		// 72.0 → log10 ≈ 1.86 → floor = 1 → 10^(1-6) = 10^-5 = 0.00001
		QCOMPARE(ccGlobalShiftManager::BestScale(72.0), 0.00001);
	}

	void testGlobalShiftTemplate()
	{
		// Store a known shift/scale pair
		CCVector3d shift(100.0, -200.0, 50.0);
		double     scale = 0.001;
		ccGlobalShiftManager::StoreShift(shift, scale);

		// Retrieve it via GetLast
		const std::vector<ccGlobalShiftManager::ShiftInfo>& history = ccGlobalShiftManager::GetLast();
		QVERIFY(!history.empty());

		const ccGlobalShiftManager::ShiftInfo& last = history.back();
		QCOMPARE(last.shift.x, 100.0);
		QCOMPARE(last.shift.y, -200.0);
		QCOMPARE(last.shift.z, 50.0);
		QCOMPARE(last.scale, 0.001);
	}
};

QTEST_APPLESS_MAIN(TestGlobalShift)

#include "TestGlobalShift.moc"
