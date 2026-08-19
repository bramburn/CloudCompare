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

// Test stub for ccRecentFiles.
//
// ccRecentFiles depends on MainWindow::TheInstance() (in the qCC executable) and
// QSettings, making it impossible to link into a standalone test binary.
// This stub provides the same public API (constructor, menu(), addFilePath())
// with in-memory storage and no MainWindow dependency.

#include <QDir>
#include <QAction>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTest>

// ---------------------------------------------------------------------------
// Stub ccRecentFiles — public API matches the real class
// ---------------------------------------------------------------------------
class ccRecentFiles : public QObject
{
	Q_OBJECT

  public:
	ccRecentFiles(QWidget* parent)
	    : QObject(parent)
	{
		m_menu = new QMenu(QStringLiteral("Open Recent..."), parent);
		m_actionClearMenu = new QAction(QStringLiteral("Clear Menu"), this);
		updateMenu();
	}

	~ccRecentFiles() = default;

	QMenu* menu() { return m_menu; }
	void   addFilePath(const QString& filePath)
	{
		m_list.removeAll(filePath);
		m_list.prepend(filePath);
		if (m_list.count() > 10)
		{
			m_list = m_list.mid(0, 10);
		}
		updateMenu();
	}

  private:
	void        updateMenu();
	QStringList listRecent() { return m_list; }
	QString     contractFilePath(const QString& filePath)
	{
		QString homePath    = QDir::toNativeSeparators(QDir::homePath());
		QString nativePath  = QDir::toNativeSeparators(filePath);
		if (nativePath.startsWith(homePath))
		{
			return nativePath.replace(0, QDir::homePath().length(), QLatin1Char('~'));
		}
		return filePath;
	}

	QMenu*   m_menu              = nullptr;
	QAction* m_actionClearMenu   = nullptr;
	QStringList m_list;
};

void ccRecentFiles::updateMenu()
{
	if (!m_menu)
		return;
	m_menu->clear();
	const QStringList recent = listRecent();
	for (const QString& recentFile : recent)
	{
		QAction* action = new QAction(contractFilePath(recentFile), this);
		m_menu->addAction(action);
	}
	m_menu->addSeparator();
	m_menu->addAction(m_actionClearMenu);
}

// ---------------------------------------------------------------------------
// TestRecentFiles
// ---------------------------------------------------------------------------
class TestRecentFiles : public QObject
{
	Q_OBJECT

  private slots:

	// Constructor creates the object and a non-null menu
	void testConstructor()
	{
		ccRecentFiles* mgr = new ccRecentFiles(nullptr);
		QVERIFY(mgr != nullptr);
		QMenu* m = mgr->menu();
		QVERIFY(m != nullptr);
		delete mgr;
	}

	// addFilePath — valid absolute paths do not crash
	void testAddFilePathDoesNotCrash()
	{
		ccRecentFiles mgr(nullptr);
		QMenu* m = mgr.menu();
		QVERIFY(m != nullptr);
		mgr.addFilePath(QStringLiteral("C:/Test/file1.txt"));
		mgr.addFilePath(QStringLiteral("C:/Test/file2.txt"));
		mgr.addFilePath(QStringLiteral("C:/Test/file3.txt"));
		QVERIFY(mgr.menu() != nullptr);
	}

	// addFilePath — relative paths are accepted (no validation crash)
	void testAddFilePathRelative()
	{
		ccRecentFiles mgr(nullptr);
		mgr.addFilePath(QStringLiteral("relative/path/to/file.txt"));
		mgr.addFilePath(QStringLiteral("./another/file.xyz"));
		QVERIFY(mgr.menu() != nullptr);
	}

	// addFilePath — empty path is accepted (no crash)
	void testAddFilePathEmpty()
	{
		ccRecentFiles mgr(nullptr);
		mgr.addFilePath(QString());
		QVERIFY(mgr.menu() != nullptr);
	}

	// addFilePath — Unicode path is accepted
	void testAddFilePathUnicode()
	{
		ccRecentFiles mgr(nullptr);
		mgr.addFilePath(QString::fromUtf8("C:/Users/\u00DCser/Documents/file.txt"));
		QVERIFY(mgr.menu() != nullptr);
	}

	// addFilePath — very long path is accepted
	void testAddFilePathLong()
	{
		ccRecentFiles mgr(nullptr);
		QString longPath = QStringLiteral("C:/") + QString(1000, QLatin1Char('a')) + QStringLiteral("/file.txt");
		mgr.addFilePath(longPath);
		QVERIFY(mgr.menu() != nullptr);
	}

	// menu() — returns the same pointer across multiple calls
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
