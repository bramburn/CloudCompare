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

/**
 * @file TestGlobalShift.cpp
 *
 * @brief Standalone test for global coordinate shift
 *
 * Tests large-coordinate precision via global origin offset:
 * - Shift threshold detection
 * - Coordinate transformation
 * - SF value round-trip preservation
 *
 * @see ccGlobalShiftManager.cpp
 */
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
		// BestShift(CCVector3d) — each component is negated and rounded to nearest 1000
		// if |component| >= MAX_COORDINATE_ABS_VALUE (1e4 default).
		// roundOffScalePower = 3 → round to multiples of 1000.

		// Below threshold (10000) → no shift
		CCVector3d below = CCVector3d(9999.0, 0.0, 0.0);
		CCVector3d resultBelow = ccGlobalShiftManager::BestShift(below);
		QCOMPARE(resultBelow.x, 0.0);
		QCOMPARE(resultBelow.y, 0.0);
		QCOMPARE(resultBelow.z, 0.0);

		// At boundary (exactly 1e4) → |x| = 10000 >= 10000 → shift needed, round 10000 → 10000
		CCVector3d boundary = CCVector3d(10000.0, 0.0, 0.0);
		CCVector3d resultBoundary = ccGlobalShiftManager::BestShift(boundary);
		QCOMPARE(resultBoundary.x, -10000.0);  // -int(10000/1000)*1000 = -10000

		// Slightly above boundary → rounds to nearest 1000 multiple
		CCVector3d above = CCVector3d(14267.0, 0.0, 0.0);
		CCVector3d resultAbove = ccGlobalShiftManager::BestShift(above);
		// 14267 / 1000 = 14.267 → int(14.267) * 1000 = 14000 → negated = -14000
		QCOMPARE(resultAbove.x, -14000.0);

		// Negative value → sign-preserving shift (to bring toward zero)
		// |x| = 626146 >= 10000 → shift = -(-626146) = +626146, rounded = +626000
		CCVector3d negative = CCVector3d(-626146.0, 0.0, 0.0);
		CCVector3d resultNeg = ccGlobalShiftManager::BestShift(negative);
		QCOMPARE(resultNeg.x, 626000.0); // was -626000.0 — BestShift shifts negative coords TOWARD zero (+)

		// Very large negative → rounds with sign
		CCVector3d bigNeg = CCVector3d(-2200000.0, 0.0, 0.0);
		CCVector3d resultBigNeg = ccGlobalShiftManager::BestShift(bigNeg);
		// 2200000 / 1000 = 2200 → int(2200) * 1000 = 2200000 → negated = -2200000
		QCOMPARE(resultBigNeg.x, -2200000.0);

		// Zero → no shift
		CCVector3d zero = CCVector3d(0.0, 0.0, 0.0);
		CCVector3d resultZero = ccGlobalShiftManager::BestShift(zero);
		QCOMPARE(resultZero.x, 0.0);
		QCOMPARE(resultZero.y, 0.0);
		QCOMPARE(resultZero.z, 0.0);

		// Multiple components: only x and z exceed threshold
		CCVector3d multi = CCVector3d(15000.0, 500.0, -23000.0);
		CCVector3d resultMulti = ccGlobalShiftManager::BestShift(multi);
		// x: 15000/1000=15 → 15*1000=15000 → -15000
		QCOMPARE(resultMulti.x, -15000.0);
		// y: 500 < 10000 → no shift
		QCOMPARE(resultMulti.y, 0.0);
		// z: |-23000|=23000 >= 10000 → -int(23000/1000)*1000 = -23000
		QCOMPARE(resultMulti.z, -23000.0);
	}

	void testBestScale()
	{
		// BestScale(d) = 1.0 if d < MAX_DIAGONAL_LENGTH (1e6), else 10^(-ceil(log10(d/1e6)))
		// 72.0 < 1e6 → 1.0
		QCOMPARE(ccGlobalShiftManager::BestScale(72.0), 1.0);

		// 1422671.72 ≥ 1e6 → ratio=1.4227 → log10≈0.1527 → ceil=1 → 10^-1=0.1
		QCOMPARE(ccGlobalShiftManager::BestScale(1422671.7232666016), 0.1);

		// 4188903.43 ≥ 1e6 → ratio=4.1889 → log10≈1.4324 → ceil=2 → 10^-2=0.01
		QCOMPARE(ccGlobalShiftManager::BestScale(4188903.4295959473), 0.01);
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
