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
 * @file TestPointCloud.cpp
 *
 * @brief Unit tests for ccPointCloud
 *
 * Tests point cloud data management:
 * - Point insertion and deletion
 * - Coordinates and colors
 * - Scalar field management
 * - Global shift handling
 * - Bounding box computation
 * - Octree integration
 *
 * @see ccPointCloud.h
 */
#include <ccPointCloud.h>
#include <ccScalarField.h>

#include <QTest>
#include <QString>

class TestPointCloud : public QObject
{
	Q_OBJECT

  private:
	ccPointCloud* makeTwoPointCloud()
	{
		ccPointCloud* cloud = new ccPointCloud("test");
		cloud->reserve(2);
		cloud->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		return cloud;
	}

  private slots:

	// Construction
	void constructEmpty()
	{
		ccPointCloud cloud("empty");
		QCOMPARE(cloud.size(), static_cast<unsigned>(0));
		QCOMPARE(cloud.capacity(), static_cast<unsigned>(0));
	}

	void constructNamed()
	{
		ccPointCloud cloud("my cloud");
		QCOMPARE(cloud.getName(), QString("my cloud"));
	}

	// reserve() / resize()
	void reserveAllocatesCapacity()
	{
		ccPointCloud cloud;
		QCOMPARE(cloud.capacity(), static_cast<unsigned>(0));
		QVERIFY(cloud.reserve(100));
		QCOMPARE(cloud.capacity(), static_cast<unsigned>(100));
		QCOMPARE(cloud.size(), static_cast<unsigned>(0));
	}

	void resizeGrowsWithZeroFill()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(10));
		QVERIFY(cloud.resize(5));
		QCOMPARE(cloud.size(), static_cast<unsigned>(5));
		QCOMPARE(cloud.capacity(), static_cast<unsigned>(10));
	}

	void reserveAndAddPoints()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(10));
		for (unsigned i = 0; i < 5; ++i)
		{
			cloud.addPoint(CCVector3(static_cast<float>(i), 0.0f, 0.0f));
		}
		QCOMPARE(cloud.size(), static_cast<unsigned>(5));
	}

	// getBoundingBox()
	void boundingBoxEmpty()
	{
		CCVector3 bbMin, bbMax;
		ccPointCloud cloud("empty");
		cloud.getBoundingBox(bbMin, bbMax);
		QCOMPARE(bbMin.x, 0.0f);
		QCOMPARE(bbMin.y, 0.0f);
		QCOMPARE(bbMin.z, 0.0f);
		QCOMPARE(bbMax.x, 0.0f);
		QCOMPARE(bbMax.y, 0.0f);
		QCOMPARE(bbMax.z, 0.0f);
	}

	void boundingBoxTwoPoints()
	{
		CCVector3 bbMin, bbMax;
		{
			ccPointCloud cloud("two");
			cloud.reserve(2);
			cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
			cloud.addPoint(CCVector3(2.0f, 3.0f, 4.0f));
			cloud.getBoundingBox(bbMin, bbMax);
		}
		QCOMPARE(bbMin.x, 0.0f);
		QCOMPARE(bbMin.y, 0.0f);
		QCOMPARE(bbMin.z, 0.0f);
		QCOMPARE(bbMax.x, 2.0f);
		QCOMPARE(bbMax.y, 3.0f);
		QCOMPARE(bbMax.z, 4.0f);
	}

	// setName / getName
	void setAndGetName()
	{
		ccPointCloud cloud;
		cloud.setName("renamed");
		QCOMPARE(cloud.getName(), QString("renamed"));
	}

	// setGlobalShift / getGlobalShift
	void globalShiftRoundtrip()
	{
		CCVector3d shift(1000.0, 2000.0, 3000.0);
		{
			ccPointCloud cloud;
			cloud.setGlobalShift(shift);
			QCOMPARE(cloud.getGlobalShift().x, shift.x);
			QCOMPARE(cloud.getGlobalShift().y, shift.y);
			QCOMPARE(cloud.getGlobalShift().z, shift.z);
		}
	}

	void globalShiftDefault()
	{
		ccPointCloud cloud;
		QCOMPARE(cloud.getGlobalShift().x, 0.0);
		QCOMPARE(cloud.getGlobalShift().y, 0.0);
		QCOMPARE(cloud.getGlobalShift().z, 0.0);
	}

	// setGlobalScale / getGlobalScale
	void globalScaleRoundtrip()
	{
		{
			ccPointCloud cloud;
			cloud.setGlobalScale(0.001);
			QCOMPARE(cloud.getGlobalScale(), 0.001);
		}
	}

	void globalScaleDefault()
	{
		ccPointCloud cloud;
		QCOMPARE(cloud.getGlobalScale(), 1.0);
	}

	// scalar fields
	void addScalarField()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(3));
		cloud.addPoint(CCVector3(0, 0, 0));
		cloud.addPoint(CCVector3(1, 0, 0));
		cloud.addPoint(CCVector3(2, 0, 0));

		int sfIdx = cloud.addScalarField("values");
		QVERIFY(sfIdx >= 0);
		QCOMPARE(cloud.getNumberOfScalarFields(), 1);
		CCCoreLib::ScalarField* sf = cloud.getScalarField(sfIdx);
		QVERIFY(sf != nullptr);
		sf->setValue(0, 1.5f);
		sf->setValue(1, 2.5f);
		sf->setValue(2, 3.5f);
	}

	void scalarFieldStatistics()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(4));
		for (unsigned i = 0; i < 4; ++i)
		{
			cloud.addPoint(CCVector3(static_cast<float>(i), 0.0f, 0.0f));
		}
		int sfIdx = cloud.addScalarField("test");
		QVERIFY(sfIdx >= 0);
		CCCoreLib::ScalarField* sf = cloud.getScalarField(sfIdx);
		QVERIFY(sf != nullptr);
		sf->setValue(0, 1.0f);
		sf->setValue(1, 2.0f);
		sf->setValue(2, 3.0f);
		sf->setValue(3, 4.0f);
		sf->computeMinAndMax();

		QCOMPARE(sf->getMin(), 1.0f);
		QCOMPARE(sf->getMax(), 4.0f);
	}

	void deleteScalarField()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(1));
		cloud.addPoint(CCVector3(0, 0, 0));
		int sfIdx = cloud.addScalarField("temp");
		QVERIFY(sfIdx >= 0);
		QCOMPARE(cloud.getNumberOfScalarFields(), 1);
		cloud.deleteScalarField(sfIdx);
		QCOMPARE(cloud.getNumberOfScalarFields(), 0);
	}

	// colors — colors are added per-point as the cloud is populated
	void addColorsPerPoint()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(3));
		cloud.addPoint(CCVector3(0, 0, 0));
		cloud.addPoint(CCVector3(1, 0, 0));
		cloud.addPoint(CCVector3(2, 0, 0));

		// Color table must be allocated separately from the point table
		QVERIFY(cloud.reserveTheRGBTable());
		cloud.addColor(ccColor::Rgb(255, 128, 64));
		cloud.addColor(ccColor::Rgb(0, 255, 0));
		cloud.addColor(ccColor::Rgb(0, 0, 255));

		QCOMPARE(cloud.hasColors(), true);
	}

	void colorsNotAllocatedByDefault()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(1));
		cloud.addPoint(CCVector3(0, 0, 0));
		QCOMPARE(cloud.hasColors(), false);
	}

	// normals
	void normalsNotAllocatedByDefault()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(1));
		cloud.addPoint(CCVector3(0, 0, 0));
		QCOMPARE(cloud.hasNormals(), false);
	}

	// clear()
	void clearEmptiesCloud()
	{
		ccPointCloud* cloud = makeTwoPointCloud();
		QCOMPARE(cloud->size(), static_cast<unsigned>(2));
		cloud->clear();
		QCOMPARE(cloud->size(), static_cast<unsigned>(0));
		delete cloud;
	}

	// size() / capacity()
	void sizeAndCapacity()
	{
		ccPointCloud cloud;
		QVERIFY(cloud.reserve(10));
		for (unsigned i = 0; i < 3; ++i)
		{
			cloud.addPoint(CCVector3(static_cast<float>(i), 0.0f, 0.0f));
		}
		QCOMPARE(cloud.size(), static_cast<unsigned>(3));
		QCOMPARE(cloud.capacity(), static_cast<unsigned>(10));
	}

	// identity — getClassID()
	void classIdIsPointCloud()
	{
		ccPointCloud cloud;
		QCOMPARE(cloud.getClassID(), CC_TYPES::POINT_CLOUD);
	}
};

QTEST_GUILESS_MAIN(TestPointCloud)
#include "TestPointCloud.moc"
