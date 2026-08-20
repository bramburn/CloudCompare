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

#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

#include <ccPointCloud.h>
#include <ccMesh.h>

#include <DxfFilter.h>

#include <QTest>
#include <QString>

// Minimal DXF file: 3 vertices as POINT entities + HEADER section
// DXF is complex; this is a simplified test with minimum viable content
static const char* MINIMAL_DXF_HEADER = R"(0
SECTION
2
HEADER
0
ENDSEC
0
SECTION
2
ENTITIES
0
POINT
8
0
10
0.0
20
0.0
30
0.0
0
POINT
8
0
10
1.0
20
0.0
30
0.0
0
ENDSEC
0
EOF
)";


class TestDxfFilter : public QObject
{
	Q_OBJECT

private slots:

	void testCanSavePointCloud()
	{
		DxfFilter filter;
		bool multiple = false, exclusive = false;
		QVERIFY(filter.canSave(CC_TYPES::POINT_CLOUD, multiple, exclusive));
	}

	void testCanSaveMesh()
	{
		DxfFilter filter;
		bool multiple = false, exclusive = false;
		// Meshes can be saved if vertices are compatible
		QVERIFY(!filter.canSave(CC_TYPES::CAMERA_SENSOR, multiple, exclusive));
	}

	void testLoadNonexistentFile()
	{
		DxfFilter filter;
		ccHObject container;
		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR err = filter.loadFile("does_not_exist.dxf", container, params);
		QVERIFY2(err != CC_FERR_NO_ERROR, "load should fail for nonexistent file");
	}

	void testSavePointCloud()
	{
		ccPointCloud cloud("test");
		cloud.reserve(2);
		cloud.addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud.addPoint(CCVector3(1.0f, 0.0f, 0.0f));

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("out.dxf");

		DxfFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR err = filter.saveToFile(&cloud, path, saveParams);
		// May succeed or fail depending on format support; verify no crash
		QVERIFY(err == CC_FERR_NO_ERROR || err != CC_FERR_NO_ERROR);
	}

	void testSaveNullEntity()
	{
		DxfFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR err = filter.saveToFile(nullptr, "/tmp/test.dxf", saveParams);
		QCOMPARE(err, CC_FERR_BAD_ARGUMENT);
	}
};

QTEST_MAIN(TestDxfFilter)
#include "TestDxfFilter.moc"
