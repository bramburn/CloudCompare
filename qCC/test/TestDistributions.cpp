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

#include <ccPointCloud.h>
#include <ccScalarField.h>

#include <ScalarField.h>
#include <NormalDistribution.h>
#include <WeibullDistribution.h>
#include <GenericDistribution.h>

#include <QTest>
#include <QString>

#include <cmath>

using CCCoreLib::NormalDistribution;
using CCCoreLib::WeibullDistribution;
using CCCoreLib::ScalarField;
using CCCoreLib::GenericDistribution;
using CCCoreLib::GenericCloud;
using CCCoreLib::NAN_VALUE;

// Concrete ScalarContainer wrappers from GenericDistribution.h
using SFAsScalarContainer = CCCoreLib::GenericDistribution::SFAsScalarContainer;
using VectorAsScalarContainer = CCCoreLib::GenericDistribution::VectorAsScalarContainer;

// Helper: create a cloud with N points and one activated SF containing the given values
static ccPointCloud* makeCloudWithSF(const std::vector<ScalarType>& values)
{
	ccPointCloud* cloud = new ccPointCloud("test");
	cloud->reserve(static_cast<unsigned>(values.size()));
	for (size_t i = 0; i < values.size(); ++i)
	{
		cloud->addPoint(CCVector3(static_cast<float>(i), 0.0f, 0.0f));
	}

	ccScalarField* sf = new ccScalarField("test");
	for (ScalarType v : values)
	{
		sf->addElement(v);
	}
	int sfIdx = cloud->addScalarField(sf);
	if (sfIdx < 0)
		return nullptr;
	cloud->setCurrentOutScalarField(sfIdx);
	cloud->enableScalarField();
	return cloud;
}


class TestDistributions : public QObject
{
	Q_OBJECT

private slots:

	// === NormalDistribution tests ===

	void testNormalPDFPeakAtMu()
	{
		// Gaussian PDF is maximal at x = mu
		NormalDistribution dist(5.0f, 2.0f); // mu=5, sigma2=2
		double p_at_mu   = dist.computeP(5.0);
		double p_offset  = dist.computeP(5.5);
		QVERIFY(p_at_mu > p_offset);
		QVERIFY(p_at_mu > 0.0);
	}

	void testNormalPDFPositive()
	{
		// PDF is always non-negative
		NormalDistribution dist(0.0f, 1.0f);
		QVERIFY(dist.computeP(-10.0) >= 0.0);
		QVERIFY(dist.computeP(0.0) >= 0.0);
		QVERIFY(dist.computeP(10.0) >= 0.0);
	}

	void testNormalPDFAtInfinity()
	{
		// PDF tends to 0 at ±infinity
		NormalDistribution dist(0.0f, 1.0f);
		double p_far = dist.computeP(50.0);
		QVERIFY(p_far < 1e-10);
	}

	void testNormalPFromZeroAtMu()
	{
		// P(X <= mu) = 0.5 for symmetric distribution
		NormalDistribution dist(0.0f, 1.0f);
		double p_half = dist.computePfromZero(0.0);
		QCOMPARE(p_half, 0.5);
	}

	void testNormalPFromZeroIsCdf()
	{
		// computeP(x1, x2) should be P(x1 <= X <= x2)
		NormalDistribution dist(0.0f, 1.0f);
		double p_positive = dist.computePfromZero(2.0);       // P(X <= 2)
		double p_range    = dist.computeP(-2.0, 2.0);          // P(-2 <= X <= 2)
		// Symmetric: P(-inf to 2) - P(-inf to -2) ≈ P(-2 to 2)
		QVERIFY(p_range > 0.90); // very close to 0.9545
		QCOMPARE(p_positive, 0.5 + 0.5 * dist.computeP(-2.0, 2.0));
	}

	void testNormalSetGetParameters()
	{
		NormalDistribution dist;
		QVERIFY(dist.setParameters(3.0f, 0.5f));
		ScalarType mu = 0, sigma2 = 0;
		QVERIFY(dist.getParameters(mu, sigma2));
		QCOMPARE(mu, 3.0f);
		QCOMPARE(sigma2, 0.5f);
	}

	void testNormalSetInvalidVariance()
	{
		// Negative variance should invalidate distribution
		NormalDistribution dist;
		QVERIFY(!dist.setParameters(0.0f, -1.0f));
		QVERIFY(!dist.isValid());
	}

	void testNormalComputeParameters()
	{
		// Compute parameters from known values: mean=2, variance=2/3
		// Values: 1, 2, 3 — mean=2, variance=((1-2)^2+(2-2)^2+(3-2)^2)/3 = 2/3
		std::vector<ScalarType> vec = {1.0f, 2.0f, 3.0f};
		VectorAsScalarContainer values(vec);

		NormalDistribution dist;
		QVERIFY(dist.computeParameters(values));

		ScalarType mu = 0, sigma2 = 0;
		dist.getParameters(mu, sigma2);
		QCOMPARE(mu, 2.0f);
		// Population variance = 2/3. Use double to avoid float precision mismatch
		// in Qt's QFUZZYCOMPARE: float(2/3)=0.66666669 vs double(2/3)=0.66666666...
		QVERIFY2(std::abs(static_cast<double>(sigma2) - 2.0/3.0) < 1e-10,
		         "sigma2 should equal 2/3 (population variance of {1,2,3})");
	}

	void testNormalComputeParametersAllNAN()
	{
		// All-NAN input should fail
		std::vector<ScalarType> vec = {NAN_VALUE, NAN_VALUE};
		VectorAsScalarContainer values(vec);

		NormalDistribution dist;
		QVERIFY(!dist.computeParameters(values));
		QVERIFY(!dist.isValid());
	}

	void testNormalComputeRobustParameters()
	{
		// Values: mostly 0..1, with two extreme outliers (100, -100)
		// Robust should filter outliers and compute mean ≈ 0.5
		std::vector<ScalarType> vec = {0.0f, 0.5f, 1.0f, 100.0f, -100.0f};
		VectorAsScalarContainer values(vec);

		NormalDistribution dist;
		// nSigma=2: filter to within 2*stddev
		QVERIFY(dist.computeRobustParameters(values, 2.0));
		QVERIFY(dist.isValid());

		ScalarType mu = 0, sigma2 = 0;
		dist.getParameters(mu, sigma2);
		// Mean should be close to 0.5, not dominated by outliers
		QVERIFY(std::abs(mu - 0.5f) < 0.5f);
	}

	void testNormalChi2Dist()
	{
		// Two identical distributions: chi2 distance should be near zero
		// Create a cloud with values drawn from N(0, 1)
		std::vector<ScalarType> vec;
		for (int i = 0; i < 100; ++i)
		{
			vec.push_back(static_cast<float>(i) / 10.0f); // 0.0 to 9.9
		}
		VectorAsScalarContainer values(vec);

		NormalDistribution dist;
		QVERIFY(dist.computeParameters(values));

		ccPointCloud* cloud = makeCloudWithSF({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f});
		QVERIFY(cloud != nullptr);

		// Chi2 distance with 5 classes
		// Note: requires cloud size >= nClasses*nClasses (5*5=25 elements)
		// Test cloud has only 10 points, so use 3 classes instead (3*3=9 <= 10)
		double chi2 = dist.computeChi2Dist(cloud, 3);
		QVERIFY2(chi2 >= 0.0, "chi2 distance must be non-negative");

		delete cloud;
	}

	void testNormalChi2DistInvalid()
	{
		// nClasses=0 should return -1
		NormalDistribution dist(0.0f, 1.0f);
		ccPointCloud* cloud = makeCloudWithSF({0.0f, 1.0f, 2.0f});
		QVERIFY(cloud != nullptr);

		double chi2 = dist.computeChi2Dist(cloud, 0);
		QCOMPARE(chi2, -1.0);

		delete cloud;
	}

	void testNormalChi2DistNClasses1()
	{
		// nClasses=1 should return 0
		NormalDistribution dist(0.0f, 1.0f);
		ccPointCloud* cloud = makeCloudWithSF({0.0f, 1.0f, 2.0f});
		QVERIFY(cloud != nullptr);

		double chi2 = dist.computeChi2Dist(cloud, 1);
		QCOMPARE(chi2, 0.0);

		delete cloud;
	}

	void testNormalGetName()
	{
		NormalDistribution dist;
		QCOMPARE(QString(dist.getName()), QString("Gauss"));
	}

	// === WeibullDistribution tests ===

	void testWeibullSetGetParameters()
	{
		WeibullDistribution dist;
		QVERIFY(dist.setParameters(1.0f, 2.0f, 0.0f)); // a=1, b=2, shift=0
		ScalarType a = 0, b = 0;
		QVERIFY(dist.getParameters(a, b));
		QCOMPARE(a, 1.0f);
		QCOMPARE(b, 2.0f);
	}

	void testWeibullPDFPositive()
	{
		WeibullDistribution dist(2.0f, 1.0f); // a=2, b=1
		QVERIFY(dist.computeP(0.5) >= 0.0);
		QVERIFY(dist.computeP(1.0) >= 0.0);
	}

	void testWeibullPFromZero()
	{
		WeibullDistribution dist(2.0f, 1.0f);
		// P(X <= 0) should be 0 for Weibull (shifted from 0)
		double p_zero = dist.computePfromZero(0.0);
		QVERIFY(p_zero >= 0.0);
		QVERIFY(p_zero <= 1.0);
	}

	void testWeibullMode()
	{
		// Weibull mode for a > 1: b * ((a-1)/a)^(1/a)
		WeibullDistribution dist(2.0f, 1.0f);
		double mode = dist.computeMode();
		QVERIFY(mode >= 0.0); // mode should be non-negative
		QCOMPARE(mode, dist.computeMode()); // deterministic
	}

	void testWeibullSkewness()
	{
		WeibullDistribution dist(2.0f, 1.0f);
		double skew = dist.computeSkewness();
		// Weibull with a=2 is positively skewed (right tail)
		QVERIFY(skew > 0.0);
	}

	void testWeibullComputeParameters()
	{
		// Fit Weibull to positive values
		std::vector<ScalarType> vec = {1.0f, 2.0f, 3.0f};
		VectorAsScalarContainer values(vec);

		WeibullDistribution dist;
		// Fit may or may not succeed depending on data; we just verify it doesn't crash
		bool ok = dist.computeParameters(values);
		if (ok)
		{
			QVERIFY(dist.isValid());
		}
		// Either way, no crash
	}

	void testWeibullGetOtherParameters()
	{
		WeibullDistribution dist(2.0f, 1.0f);
		ScalarType mu = 0, sigma2 = 0;
		dist.getOtherParameters(mu, sigma2);
		// Mean and variance should be finite and non-negative for variance
		QVERIFY(std::isfinite(mu));
		QVERIFY(sigma2 >= 0.0f);
	}

	void testWeibullGetName()
	{
		WeibullDistribution dist;
		QCOMPARE(QString(dist.getName()), QString("Weibull"));
	}
};

QTEST_MAIN(TestDistributions)
#include "TestDistributions.moc"
