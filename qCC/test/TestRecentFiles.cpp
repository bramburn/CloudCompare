// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.             #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                       #
// #                                                                        #
// ##########################################################################

#include <ccRecentFiles.h>

#include <QDir>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QTest>

class TestRecentFiles : public QObject
{
	Q_OBJECT

  private slots:

	// -----------------------------------------------------------------------
	// ccRecentFiles(QWidget* parent) — constructor creates the object.
	// -----------------------------------------------------------------------
	void testConstructor()
	{
		// Create with a null parent (safe for unit testing)
		ccRecentFiles* mgr = new ccRecentFiles(nullptr);
		QVERIFY(mgr != nullptr);

		// menu() returns a non-null QMenu
		QMenu* m = mgr->menu();
		QVERIFY(m != nullptr);

		delete mgr;
	}

	// -----------------------------------------------------------------------
	// addFilePath — adding a valid absolute path does not crash.
	// -----------------------------------------------------------------------
	void testAddFilePathDoesNotCrash()
	{
		ccRecentFiles mgr(nullptr);
		QMenu* m = mgr.menu();
		QVERIFY(m != nullptr);

		// Add several paths — none should throw
		mgr.addFilePath("C:/Test/file1.txt");
		mgr.addFilePath("C:/Test/file2.txt");
		mgr.addFilePath("C:/Test/file3.txt");

		// Menu should be non-null after adding (no crash)
		QVERIFY(mgr.menu() != nullptr);
	}

	// -----------------------------------------------------------------------
	// addFilePath — relative paths are accepted (no validation crash).
	// -----------------------------------------------------------------------
	void testAddFilePathRelative()
	{
		ccRecentFiles mgr(nullptr);
		mgr.addFilePath("relative/path/to/file.txt");
		mgr.addFilePath("./another/file.xyz");
		QVERIFY(mgr.menu() != nullptr);
	}

	// -----------------------------------------------------------------------
	// addFilePath — empty path is accepted (no crash).
	// -----------------------------------------------------------------------
	void testAddFilePathEmpty()
	{
		ccRecentFiles mgr(nullptr);
		mgr.addFilePath(QString());
		QVERIFY(mgr.menu() != nullptr);
	}

	// -----------------------------------------------------------------------
	// addFilePath — Unicode path is accepted.
	// -----------------------------------------------------------------------
	void testAddFilePathUnicode()
	{
		ccRecentFiles mgr(nullptr);
		mgr.addFilePath(QString::fromUtf8("C:/Users/\u00DCser/Documents/file.txt"));
		QVERIFY(mgr.menu() != nullptr);
	}

	// -----------------------------------------------------------------------
	// addFilePath — very long path is accepted.
	// -----------------------------------------------------------------------
	void testAddFilePathLong()
	{
		ccRecentFiles mgr(nullptr);
		QString longPath = "C:/" + QString(1000, 'a') + "/file.txt";
		mgr.addFilePath(longPath);
		QVERIFY(mgr.menu() != nullptr);
	}

	// -----------------------------------------------------------------------
	// menu() — returns the same pointer across multiple calls.
	// -----------------------------------------------------------------------
	void testMenuIsSamePointer()
	{
		ccRecentFiles mgr(nullptr);
		QMenu* m1 = mgr.menu();
		QMenu* m2 = mgr.menu();
		QCOMPARE(m1, m2);
	}
};

QTEST_MAIN(TestRecentFiles)
#include "TestRecentFiles.moc"
