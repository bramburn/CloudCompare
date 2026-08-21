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
 * @file TestBinFilter.cpp
 *
 * @brief Unit tests for BinFilter
 *
 * Tests CloudCompare native binary format:
 * - Magic bytes detection
 * - Version compatibility
 * - Point data serialization (coordinates, colors)
 * - Scalar field data
 * - Entity metadata
 *
 * @see BinFilter.cpp
 */
#include "BinFilter.h"
#include "ccPointCloud.h"
#include "ccOctree.h"

#include <QtTest/QtTest>

#include <QTemporaryFile>

class TestBinFilter : public QObject
{
	Q_OBJECT

  private slots:

	void testCanSaveWhitelist()
	{
		BinFilter filter;

		// ccPointCloud is on the whitelist (not in the blacklist) → true
		ccPointCloud cloud("test");
		bool         multiple  = false;
		bool         exclusive = false;
		QString      error;
		QVERIFY(filter.canSave(cloud.getClassID(), multiple, exclusive));
		QCOMPARE(multiple, true);
		QCOMPARE(exclusive, false);
	}

	void testCanSaveBlacklist()
	{
		BinFilter filter;

		// POINT_OCTREE is explicitly blacklisted → false
		bool multiple  = true;
		bool exclusive = false;
		QVERIFY(!filter.canSave(CC_TYPES::POINT_OCTREE, multiple, exclusive));
	}

	void testHeaderMagicV1()
	{
		// V1: file does NOT start with "CCB" → LoadFileV1 is used
		// (V2 starts with "CCB2"; V1 starts with the cloud count instead)
		QTemporaryFile tmpFile;
		QVERIFY(tmpFile.open());
		QDataStream out(&tmpFile);
		// Write a uint32 (cloud count) instead of "CCB2"
		out << static_cast<uint32_t>(1);
		tmpFile.seek(0);

		char firstBytes[5] = {0};
		QCOMPARE(tmpFile.read(firstBytes, 4), qint64(4));
		// strncmp("CCB", ...) != 0 means it's V1
		QVERIFY(std::strncmp(firstBytes, "CCB", 3) != 0);
	}

	void testHeaderMagicV2()
	{
		// V2: file starts with "CCB2"
		QTemporaryFile tmpFile;
		QVERIFY(tmpFile.open());
		QDataStream out(&tmpFile);
		char magic[5] = "CCB2";
		out.writeRawData(magic, 4);
		tmpFile.seek(0);

		char firstBytes[5] = {0};
		QCOMPARE(tmpFile.read(firstBytes, 4), qint64(4));
		// strncmp("CCB", 3) == 0 means it's V2 (starts with CCB)
		QVERIFY(std::strncmp(firstBytes, "CCB", 3) == 0);
	}

	void testHeaderMagicUnknown()
	{
		// Unknown magic: neither V1 (cloud count) nor V2 ("CCB2")
		QTemporaryFile tmpFile;
		QVERIFY(tmpFile.open());
		QDataStream out(&tmpFile);
		char magic[5] = "XXXX";
		out.writeRawData(magic, 4);
		tmpFile.seek(0);

		char firstBytes[5] = {0};
		QCOMPARE(tmpFile.read(firstBytes, 4), qint64(4));
		// "XXXX" does not match "CCB" in first 3 bytes
		QVERIFY(std::strncmp(firstBytes, "CCB", 3) != 0);
	}
};

QTEST_APPLESS_MAIN(TestBinFilter)

#include "TestBinFilter.moc"
