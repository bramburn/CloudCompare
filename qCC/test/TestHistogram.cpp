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
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

#include <ccPointCloud.h>
#include <ccScalarField.h>

#include <ScalarFieldTools.h>
#include <ScalarField.h>

#include <QTest>
#include <QString>

#include <cmath>

using CCCoreLib::ScalarFieldTools;
using CCCoreLib::GenericCloud;
using CCCoreLib::NAN_VALUE;

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
	QVERIFY(sfIdx >= 0);
	cloud->setCurrentOutScalarField(sfIdx);
	cloud->enableScalarField();
	return cloud;
}

class TestHistogram : public QObject
{
	Q_OBJECT

  private slots:

	// testComputeHistogramUniform — 100 values in [0,100], 10 bins → each bin ≈ 10
	void testComputeHistogramUniform()
	{
		std::vector<ScalarType> values;
		values.reserve(100);
		for (int i = 0; i < 100; ++i)
		{
			values.push_back(static_cast<ScalarType>(i));
		}
		ccPointCloud* cloud = makeCloudWithSF(values);

		std::vector<int> histo;
		ScalarFieldTools::computeScalarFieldHistogram(cloud, 10, histo);

		// 10 bins, each roughly 10 entries (values 0-9, 10-19, …, 90-99)
		QCOMPARE(static_cast<int>(histo.size()), 10);

		// All bins should have exactly 10 entries (uniform distribution)
		for (int count : histo)
		{
			QCOMPARE(count, 10);
		}

		delete cloud;
	}

	// testHistogramSum — sum of bin counts = total number of values
	void testHistogramSum()
	{
		std::vector<ScalarType> values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
		ccPointCloud* cloud = makeCloudWithSF(values);

		std::vector<int> histo;
		ScalarFieldTools::computeScalarFieldHistogram(cloud, 5, histo);

		int sum = 0;
		for (int count : histo)
		{
			sum += count;
		}
		QCOMPARE(sum, 10); // all 10 valid values counted

		delete cloud;
	}

	// testHistogramEmpty — 0 bins → empty vector
	void testHistogramEmpty()
	{
		std::vector<ScalarType> values = {1.0, 2.0, 3.0, 4.0, 5.0};
		ccPointCloud* cloud = makeCloudWithSF(values);

		std::vector<int> histo;
		ScalarFieldTools::computeScalarFieldHistogram(cloud, 0, histo);

		QCOMPARE(static_cast<int>(histo.size()), 0);

		delete cloud;
	}

	// testHistogramSingleBin — all values identical → all go in one bin
	void testHistogramSingleBin()
	{
		std::vector<ScalarType> values(20, 5.0f); // 20 entries, all value=5
		ccPointCloud* cloud = makeCloudWithSF(values);

		std::vector<int> histo;
		ScalarFieldTools::computeScalarFieldHistogram(cloud, 10, histo);

		QCOMPARE(static_cast<int>(histo.size()), 10);

		// Only one bin should contain all 20 entries
		int nonZeroBins = 0;
		for (int count : histo)
		{
			if (count > 0)
			{
				++nonZeroBins;
				QCOMPARE(count, 20);
			}
		}
		QCOMPARE(nonZeroBins, 1);

		delete cloud;
	}

	// testHistogramNaNFiltered — values include NaN → NaN excluded from histogram
	void testHistogramNaNFiltered()
	{
		std::vector<ScalarType> values = {
		    1.0, NAN_VALUE, 3.0, NAN_VALUE, 5.0, NAN_VALUE, 7.0, NAN_VALUE, 9.0
		};
		ccPointCloud* cloud = makeCloudWithSF(values);

		std::vector<int> histo;
		ScalarFieldTools::computeScalarFieldHistogram(cloud, 5, histo);

		// 5 valid values (excluding 4 NaN), spread across 5 bins
		int sum = 0;
		for (int count : histo)
		{
			sum += count;
		}
		QCOMPARE(sum, 5); // NaN values are not counted

		delete cloud;
	}
};

QTEST_MAIN(TestHistogram)
#include "TestHistogram.moc"
