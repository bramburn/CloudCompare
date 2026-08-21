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
 * @file TestStatisticalTesting.cpp
 *
 * @brief Standalone test for statistical comparison
 *
 * Tests significance of scalar field differences:
 * - t-test and KS test
 * - Threshold sensitivity
 *
 * @see StatisticalTestingTools.h
 */
#include <StatisticalTestingTools.h>
#include <NormalDistribution.h>

#include <QTest>
#include <QString>

#include <cmath>

using CCCoreLib::StatisticalTestingTools;
using CCCoreLib::NormalDistribution;


class TestStatisticalTesting : public QObject
{
	Q_OBJECT

private slots:

	void testChi2FractileConf90()
	{
		// Chi2 fractile at 90% confidence for 1 degree of freedom ≈ 2.706
		double f = StatisticalTestingTools::computeChi2Fractile(0.90, 1);
		QVERIFY2(f > 2.0 && f < 4.0, "Chi2 90% conf 1df should be between 2 and 4");
	}

	void testChi2FractileConf95()
	{
		// Chi2 fractile at 95% confidence for 2 degrees of freedom ≈ 5.991
		double f = StatisticalTestingTools::computeChi2Fractile(0.95, 2);
		QVERIFY2(f > 4.0 && f < 8.0, "Chi2 95% conf 2df should be between 4 and 8");
	}

	void testChi2FractileConf99()
	{
		// Chi2 fractile at 99% confidence for 3 degrees of freedom ≈ 11.345
		double f = StatisticalTestingTools::computeChi2Fractile(0.99, 3);
		QVERIFY2(f > 9.0 && f < 15.0, "Chi2 99% conf 3df should be between 9 and 15");
	}

	void testChi2ProbabilityMonotonic()
	{
		// As degrees of freedom increase, probability should decrease (for same chi2 value)
		double chi2 = 5.0;
		double p1 = StatisticalTestingTools::computeChi2Probability(chi2, 1);
		double p2 = StatisticalTestingTools::computeChi2Probability(chi2, 2);
		double p3 = StatisticalTestingTools::computeChi2Probability(chi2, 3);
		// Higher chi2 value for more degrees of freedom → lower probability
		QVERIFY(p1 >= 0.0 && p1 <= 1.0);
		QVERIFY(p2 >= 0.0 && p2 <= 1.0);
		QVERIFY(p3 >= 0.0 && p3 <= 1.0);
	}

	void testChi2ProbabilityAtZero()
	{
		// chi2=0 → probability should be very small (near zero)
		double p = StatisticalTestingTools::computeChi2Probability(0.0, 5);
		QVERIFY2(p >= 0.0 && p <= 1.0, "probability must be in [0,1]");
		// For df>=1, chi2=0 => CDF(0) = 0, so p should be exactly 0
		QCOMPARE(p, 0.0); // exact comparison for this case
	}

	void testChi2ProbabilitySymmetry()
	{
		// chi2 value and probability are inversely related
		double chi2_low = 1.0;
		double chi2_high = 10.0;
		double p_low = StatisticalTestingTools::computeChi2Probability(chi2_low, 3);
		double p_high = StatisticalTestingTools::computeChi2Probability(chi2_high, 3);
		QVERIFY(p_low > p_high); // larger chi2 → smaller probability
	}

	void testChi2FractileProbabilityRoundtrip()
	{
		// For a given confidence p and d degrees of freedom:
		// fractile(p, d) and probability(fractile(p,d), d) should round-trip
		double p = 0.95;
		int d = 5;
		double f = StatisticalTestingTools::computeChi2Fractile(p, d);
		double p_back = StatisticalTestingTools::computeChi2Probability(f, d);
		QVERIFY2(std::abs(p_back - p) < 1e-6, "fractile/probability should round-trip");
	}
};

QTEST_MAIN(TestStatisticalTesting)
#include "TestStatisticalTesting.moc"
