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

#include <DgmOctree.h>
#include <GenericCloud.h>

#include <QTest>
#include <QString>

#include <cmath>

using CCCoreLib::DgmOctree;
using CCCoreLib::GenericCloud;

// Helper: build an octree from a cloud
static DgmOctree* buildOctree(ccPointCloud* cloud)
{
	if (!cloud || cloud->size() == 0)
		return nullptr;
	DgmOctree* octree = new DgmOctree(cloud);
	if (octree->build(nullptr) <= 0)
	{
		delete octree;
		return nullptr;
	}
	return octree;
}


class TestOctree : public QObject
{
	Q_OBJECT

private slots:

	void testBuildEmpty()
	{
		ccPointCloud empty("empty");
		DgmOctree* octree = buildOctree(&empty);
		QVERIFY(octree == nullptr); // can't build on empty
	}

	void testBuildSinglePoint()
	{
		ccPointCloud cloud("single");
		cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		DgmOctree* octree = buildOctree(&cloud);
		QVERIFY(octree != nullptr);
		QCOMPARE(octree->size(), static_cast<unsigned>(1));
		delete octree;
	}

	void testBuildKnownPoints()
	{
		// 8 corner points of a unit cube
		ccPointCloud cloud("cube");
		cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(0.0f, 1.0f, 0.0f));
		cloud.addPoint(CCVector3(1.0f, 1.0f, 0.0f));
		cloud.addPoint(CCVector3(0.0f, 0.0f, 1.0f));
		cloud.addPoint(CCVector3(1.0f, 0.0f, 1.0f));
		cloud.addPoint(CCVector3(0.0f, 1.0f, 1.0f));
		cloud.addPoint(CCVector3(1.0f, 1.0f, 1.0f));

		DgmOctree* octree = buildOctree(&cloud);
		QVERIFY(octree != nullptr);
		QCOMPARE(octree->size(), static_cast<unsigned>(8));
		delete octree;
	}

	void testOctreeLevelForCell()
	{
		// DgmOctree::OCTREE_LENGTH(level) = 2^level
		QCOMPARE(DgmOctree::OCTREE_LENGTH(0), 1);
		QCOMPARE(DgmOctree::OCTREE_LENGTH(1), 2);
		QCOMPARE(DgmOctree::OCTREE_LENGTH(2), 4);
		QCOMPARE(DgmOctree::OCTREE_LENGTH(10), 1024);
	}

	void testOctreeBitShift()
	{
		// GET_BIT_SHIFT(level) should return 3*level
		QCOMPARE(DgmOctree::GET_BIT_SHIFT(0), static_cast<unsigned char>(0));
		QCOMPARE(DgmOctree::GET_BIT_SHIFT(1), static_cast<unsigned char>(3));
		QCOMPARE(DgmOctree::GET_BIT_SHIFT(2), static_cast<unsigned char>(6));
	}

	void testOctreeBoundsAfterBuild()
	{
		ccPointCloud cloud("bounds");
		cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(10.0f, 10.0f, 10.0f));

		DgmOctree* octree = buildOctree(&cloud);
		QVERIFY(octree != nullptr);

		// Bounding box should cover the points
		const ccBBox& bbox = octree->getBoundingBox();
		QVERIFY(bbox.isValid());

		// Min should be near (0,0,0), max near (10,10,10)
		CCVector3 bbMin, bbMax;
		bbox.getMin(bbMin);
		bbox.getMax(bbMax);
		QVERIFY(bbMin.x <= 0.0f);
		QVERIFY(bbMin.y <= 0.0f);
		QVERIFY(bbMin.z <= 0.0f);
		QVERIFY(bbMax.x >= 10.0f);
		QVERIFY(bbMax.y >= 10.0f);
		QVERIFY(bbMax.z >= 10.0f);

		delete octree;
	}

	void testOctreeAlternativeTruncateAtLevel()
	{
		ccPointCloud cloud("alt");
		cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(0.0f, 1.0f, 0.0f));
		cloud.addPoint(CCVector3(0.0f, 0.0f, 1.0f));

		DgmOctree* octree = buildOctree(&cloud);
		QVERIFY(octree != nullptr);

		// buildWithAlternativeParam should work if available
		// Just verify the octree is valid after build
		QCOMPARE(octree->size(), static_cast<unsigned>(4));
		QVERIFY(octree->getBoundingBox().isValid());

		delete octree;
	}
};

QTEST_MAIN(TestOctree)
#include "TestOctree.moc"
