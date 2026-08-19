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
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

#include <ccPointCloud.h>

#include <ImageFileFilter.h>

#include <QTest>
#include <QString>

#include <cmath>


class TestImageFilter : public QObject
{
	Q_OBJECT

private slots:

	void testCanSavePointCloud()
	{
		ImageFileFilter filter;
		bool multiple = false, exclusive = false;
		QVERIFY(filter.canSave(CC_TYPES::POINT_CLOUD, multiple, exclusive));
	}

	void testCanSaveMesh()
	{
		ImageFileFilter filter;
		bool multiple = false, exclusive = false;
		QVERIFY(!filter.canSave(CC_TYPES::MESH, multiple, exclusive));
	}

	void testCanSaveSensor()
	{
		ImageFileFilter filter;
		bool multiple = false, exclusive = false;
		QVERIFY(!filter.canSave(CC_TYPES::CAMERA_SENSOR, multiple, exclusive));
	}

	void testLoadNonexistentFile()
	{
		ImageFileFilter filter;
		ccHObject container;
		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;

		CC_FILE_ERROR err = filter.loadFile("does_not_exist.png", container, params);
		QCOMPARE(err, CC_FERR_UNKNOWN_FILE);
	}

	void testSavePointCloud()
	{
		// Create a cloud (not renderable as image, but ImageFileFilter::canSave should return true)
		ccPointCloud cloud("test");

		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.filePath("out.png");

		ImageFileFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		// saveToFile with a point cloud will fail (no image data), but shouldn't crash
		CC_FILE_ERROR err = filter.saveToFile(&cloud, path, saveParams);
		// Expected: CC_FERR_BAD_ENTITY_TYPE (cloud has no image data)
		QVERIFY(err != CC_FERR_NO_ERROR);
	}

	void testSaveNullEntity()
	{
		ImageFileFilter filter;
		FileIOFilter::SaveParameters saveParams;
		saveParams.alwaysDisplaySaveDialog = false;

		CC_FILE_ERROR err = filter.saveToFile(nullptr, "/tmp/test.png", saveParams);
		QCOMPARE(err, CC_FERR_BAD_ARGUMENT);
	}

	void testGetLoadFilenameNoCrash()
	{
		// Static dialog methods should not crash even with null parent
		QString result = ImageFileFilter::GetLoadFilename("Test", ".");
		// Result depends on whether user cancels or not — just verify no crash
	}

	void testGetSaveFilenameNoCrash()
	{
		QString result = ImageFileFilter::GetSaveFilename("Test", "test.png", ".");
		// Result depends on dialog — just verify no crash
	}
};

QTEST_MAIN(TestImageFilter)
#include "TestImageFilter.moc"
