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

#include <PlyFilter.h>

#include <QTest>
#include <QString>

#include <cmath>

// Minimal ASCII PLY — 3 vertices, x y z format
static const char* MINIMAL_ASCII_PLY = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
end_header
0.0 0.0 0.0
1.0 0.0 0.0
0.0 1.0 0.0
)";

// ASCII PLY with vertex colors
static const char* ASCII_PLY_WITH_COLOR = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
end_header
0.0 0.0 0.0 255 0 0
1.0 0.0 0.0 0 255 0
0.0 1.0 0.0 0 0 255
)";

// ASCII PLY with scalar field
static const char* ASCII_PLY_WITH_SF = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
property float scalar_values
end_header
0.0 0.0 0.0 1.0
1.0 0.0 0.0 2.0
0.0 1.0 0.0 3.0
)";


class TestPlyFilter : public QObject
{
	Q_OBJECT

private slots:

	void testLoadAsciiMinimal()
	{
		// Write minimal ASCII PLY to temp file
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("minimal.ply");

		QFile f(path);
		QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
		QTextStream s(&f);
		s << MINIMAL_ASCII_PLY;
		f.close();

		// Load
		PlyFilter filter;
		ccHObject container;
		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR err = filter.loadFile(path, container, params);
		QCOMPARE(err, CC_FERR_NO_ERROR);

		// Should have exactly one point cloud child
		QCOMPARE(container.getChildrenNumber(), 1);
		ccHObject* child = container.getChild(0);
		QVERIFY(child != nullptr);
		QVERIFY(child->isKindOf(CC_TYPES::POINT_CLOUD));

		ccPointCloud* cloud = static_cast<ccPointCloud*>(child);
		QCOMPARE(cloud->size(), static_cast<unsigned>(3));
	}

	void testLoadAsciiWithColors()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("color.ply");

		QFile f(path);
		QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
		QTextStream s(&f);
		s << ASCII_PLY_WITH_COLOR;
		f.close();

		PlyFilter filter;
		ccHObject container;
		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR err = filter.loadFile(path, container, params);
		QCOMPARE(err, CC_FERR_NO_ERROR);

		ccPointCloud* cloud = dynamic_cast<ccPointCloud*>(container.getChild(0));
		QVERIFY(cloud != nullptr);
		QCOMPARE(cloud->size(), static_cast<unsigned>(3));
		QVERIFY(cloud->hasColors());
	}

	void testLoadAsciiWithScalarField()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("sf.ply");

		QFile f(path);
		QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
		QTextStream s(&f);
		s << ASCII_PLY_WITH_SF;
		f.close();

		PlyFilter filter;
		ccHObject container;
		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR err = filter.loadFile(path, container, params);
		QCOMPARE(err, CC_FERR_NO_ERROR);

		ccPointCloud* cloud = dynamic_cast<ccPointCloud*>(container.getChild(0));
		QVERIFY(cloud != nullptr);
		QCOMPARE(cloud->size(), static_cast<unsigned>(3));
		QCOMPARE(cloud->getNumberOfScalarFields(), 1);
	}

	void testSaveLoadRoundtrip()
	{
		// Create a cloud with known points
		ccPointCloud* cloud = new ccPointCloud("roundtrip");
		cloud->reserve(3);
		cloud->addPoint(CCVector3(1.0f, 2.0f, 3.0f));
		cloud->addPoint(CCVector3(4.0f, 5.0f, 6.0f));
		cloud->addPoint(CCVector3(7.0f, 8.0f, 9.0f));

		// Save as ASCII
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("out.ply");

		PlyFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR saveErr = filter.saveToFile(cloud, path, saveParams);
		QCOMPARE(saveErr, CC_FERR_NO_ERROR);

		// Load back
		ccHObject container;
		FileIOFilter::LoadParameters loadParams;
		loadParams.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR loadErr = filter.loadFile(path, container, loadParams);
		QCOMPARE(loadErr, CC_FERR_NO_ERROR);

		QCOMPARE(container.getChildrenNumber(), 1);
		ccPointCloud* loaded = dynamic_cast<ccPointCloud*>(container.getChild(0));
		QVERIFY(loaded != nullptr);
		QCOMPARE(loaded->size(), cloud->size());

		// Check points
		const CCVector3* p0 = loaded->getPoint(0);
		QCOMPARE(p0->x, 1.0f);
		QCOMPARE(p0->y, 2.0f);
		QCOMPARE(p0->z, 3.0f);

		delete cloud;
	}

	void testSaveLoadRoundtripWithColors()
	{
		// Create cloud with colors
		ccPointCloud* cloud = new ccPointCloud("colored");
		cloud->reserve(2);
		cloud->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		cloud->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		cloud->reserveTheRGBTable();
		ccColor::Rgb c0(255, 0, 0);
		ccColor::Rgb c1(0, 255, 0);
		cloud->addColor(c0);
		cloud->addColor(c1);
		cloud->setColor(ccColor::Rgba(c0.r, c0.g, c0.b, 255));

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("color_out.ply");

		PlyFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR saveErr = filter.saveToFile(cloud, path, saveParams);
		QCOMPARE(saveErr, CC_FERR_NO_ERROR);

		// Reload
		ccHObject container;
		FileIOFilter::LoadParameters loadParams;
		loadParams.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR loadErr = filter.loadFile(path, container, loadParams);
		QCOMPARE(loadErr, CC_FERR_NO_ERROR);

		ccPointCloud* loaded = dynamic_cast<ccPointCloud*>(container.getChild(0));
		QVERIFY(loaded != nullptr);
		QVERIFY(loaded->hasColors());

		delete cloud;
	}

	void testSetDefaultOutputFormat()
	{
		PlyFilter::SetDefaultOutputFormat(PLY_ASCII);
		PlyFilter::SetDefaultOutputFormat(PLY_DEFAULT);
		// Just verify it doesn't crash
	}

	void testSetAddSFPrefix()
	{
		PlyFilter::SetAddSFPrefix(true);
		PlyFilter::SetAddSFPrefix(false);
		// Just verify it doesn't crash
	}

	void testCanSave()
	{
		PlyFilter filter;

		ccPointCloud cloud("test");
		bool multiple = false, exclusive = false;
		QVERIFY(filter.canSave(CC_TYPES::POINT_CLOUD, multiple, exclusive));
		QCOMPARE(multiple, false);
		QCOMPARE(exclusive, true);

		ccMesh mesh(nullptr);
		multiple = exclusive = true; // reset
		QVERIFY(filter.canSave(CC_TYPES::MESH, multiple, exclusive));
		QCOMPARE(multiple, false);
		QCOMPARE(exclusive, true);

		multiple = exclusive = false;
		QVERIFY(!filter.canSave(CC_TYPES::CAMERA_SENSOR, multiple, exclusive));
	}

	void testLoadNonexistentFile()
	{
		PlyFilter filter;
		ccHObject container;
		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR err = filter.loadFile("does_not_exist.ply", container, params);
		QCOMPARE(err, CC_FERR_UNKNOWN_FILE);
	}

	void testSaveWithMesh()
	{
		// Save a mesh (cloud backed by a mesh)
		ccPointCloud* vertices = new ccPointCloud("verts");
		vertices->reserve(3);
		vertices->addPoint(CCVector3(0.0f, 0.0f, 0.0f));
		vertices->addPoint(CCVector3(1.0f, 0.0f, 0.0f));
		vertices->addPoint(CCVector3(0.0f, 1.0f, 0.0f));

		ccMesh* mesh = new ccMesh(vertices);
		mesh->addTriangle(0, 1, 2);
		mesh->setName("testmesh");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("mesh.ply");

		PlyFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR saveErr = filter.saveToFile(mesh, path, saveParams);
		QCOMPARE(saveErr, CC_FERR_NO_ERROR);

		// Load it back
		ccHObject container;
		FileIOFilter::LoadParameters loadParams;
		loadParams.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR loadErr = filter.loadFile(path, container, loadParams);
		QCOMPARE(loadErr, CC_FERR_NO_ERROR);

		// Should contain a mesh with vertices
		QCOMPARE(container.getChildrenNumber(), 1);

		delete mesh;
	}

	void testSaveNullEntity()
	{
		PlyFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR err = filter.saveToFile(nullptr, "/tmp/test.ply", saveParams);
		QCOMPARE(err, CC_FERR_BAD_ARGUMENT);
	}

	void testSaveEmptyFilename()
	{
		ccPointCloud cloud("test");
		PlyFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR err = filter.saveToFile(&cloud, "", saveParams);
		QCOMPARE(err, CC_FERR_BAD_ARGUMENT);
	}
};

QTEST_MAIN(TestPlyFilter)
#include "TestPlyFilter.moc"
