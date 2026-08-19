// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU Lesser General Public License as        #
// #  published by the Free Software Foundation; version 2 or later of the   #
// #  License.                                                              #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU Lesser General Public License for more details.                   #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)            #
// #                                                                        #
// ##########################################################################

#include "AsciiFilter.h"
#include "AsciiOpenDlg.h"
#include "FileIOFilter.h"
#include "ccGlobalShiftManager.h"
#include "ccHObject.h"
#include "ccPointCloud.h"

#include <QtTest/QtTest>

#include <QBuffer>
#include <QByteArray>
#include <QCoreApplication>
#include <QString>
#include <QTemporaryDir>
#include <QTextStream>

// ##########################################################################
// Test-friend wrapper: exposes the protected loadCloudFromFormatedAsciiStream
// method so it can be exercised in tests without subclassing AsciiFilter.
// ##########################################################################
class AsciiFilterTestFriend : public AsciiFilter
{
  public:
	using AsciiFilter::loadCloudFromFormatedAsciiStream;
};

// ##########################################################################
// Helper: count how many columns in `text` contain valid doubles when split
// by `separator`.  Mirrors the logic used by AsciiOpenDlg internally.
// ##########################################################################
static size_t CountNumericColumns(const QString& text, QChar separator)
{
	QTextStream ss(&const_cast<QString&>(text), QIODevice::ReadOnly);
	QString     line = ss.readLine();
	if (line.isNull())
	{
		return 0;
	}

	// Split first, then count valid numeric columns.
	// NOTE: we intentionally do NOT use QString::simplified() here, because
	// simplified() collapses \\t (tab) to a space, making it impossible to
	// distinguish tab-separated values from space-separated values.
	QStringList parts = line.split(separator);
	size_t     validCount = 0;

	for (const QString& part : parts)
	{
		bool ok = false;
		part.toDouble(&ok);
		if (ok)
		{
			++validCount;
		}
	}
	return validCount;
}

// ##########################################################################
// Helper: find the best separator from space, comma, semicolon, and tab.
// Returns the separator that yields the most numeric columns in the first line.
// ##########################################################################
static QChar FindBestSeparator(const QString& text)
{
	const QList<QChar> separators = {QChar(' '), QChar(','), QChar(';'), QChar('\t')};

	size_t maxValidColumnCount = 0;
	QChar  bestSep           = separators.front();

	for (QChar sep : separators)
	{
		size_t count = CountNumericColumns(text, sep);
		if (count > maxValidColumnCount)
		{
			maxValidColumnCount = count;
			bestSep            = sep;
		}
	}
	return bestSep;
}

// ##########################################################################
// Test class
// ##########################################################################
class TestAsciiFilter : public QObject
{
	Q_OBJECT

  public:
	explicit TestAsciiFilter(QObject* parent = nullptr)
	    : QObject(parent)
	{
		// Ensure a QCoreApplication instance exists (required by AsciiOpenDlg
		// for screen-size queries). QtTest may have already created one.
		static int argc = 0;
		static char* argv[1] = {nullptr};
		if (QCoreApplication::instance() == nullptr)
		{
			(void) new QCoreApplication(argc, argv);
		}
	}

  private:
	// Helper: call loadCloudFromFormatedAsciiStream via test-friend wrapper.
	// Resets the container before parsing and returns the number of children.
	int ParseWith(AsciiFilterTestFriend&       af,
	              const QByteArray&           data,
	              ccHObject&                  container,
	              const AsciiOpenDlg::Sequence& seq,
	              char                         separator,
	              bool                         commaAsDecimal,
	              unsigned                     maxCloudSize,
	              unsigned                     skipLines)
	{
		container.removeAllChildren();

		FileIOFilter::LoadParameters params;
		params.alwaysDisplayLoadDialog = false;
		params.shiftHandlingMode       = ccGlobalShiftManager::Mode::NO_DIALOG;
		params._coordinatesShiftEnabled = nullptr;
		params._coordinatesShift        = nullptr;

		QTextStream ss(const_cast<QByteArray&>(data));
		ss.seek(0);

		// NOLINTNEXTLINE — test-friend grants access
		af.loadCloudFromFormatedAsciiStream(
		    ss,
		    "test",
		    container,
		    seq,
		    separator,
		    commaAsDecimal,
		    0,  // approximateNumberOfLines
		    static_cast<qint64>(data.size()),
		    maxCloudSize,
		    skipLines,
		    1.0,  // quaternionScale
		    params);

		return container.getChildrenNumber();
	}

	// Build a 3-column XYZ sequence: X, Y, Z (indices 0, 1, 2).
	AsciiOpenDlg::Sequence MakeXYZSequence()
	{
		AsciiOpenDlg::Sequence seq;
		seq.push_back({ASCII_OPEN_DLG_X, QStringLiteral("X")});
		seq.push_back({ASCII_OPEN_DLG_Y, QStringLiteral("Y")});
		seq.push_back({ASCII_OPEN_DLG_Z, QStringLiteral("Z")});
		return seq;
	}

  private slots:

	// -----------------------------------------------------------------------
	// Separator-detection tests (pure string analysis — no I/O)
	// -----------------------------------------------------------------------

	void testAutoFindBestSeparatorComma()
	{
		// In CSV, comma separates numeric values → produces 3 numeric columns.
		QString text = QStringLiteral("1.0,2.0,3.0\n4.0,5.0,6.0\n7.0,8.0,9.0");
		QCOMPARE(FindBestSeparator(text), QChar(','));
	}

	void testAutoFindBestSeparatorSemicolon()
	{
		// European CSV with semicolon → 3 numeric cols.
		QString text = QStringLiteral("1.0;2.0;3.0\n4.0;5.0;6.0");
		QCOMPARE(FindBestSeparator(text), QChar(';'));
	}

	void testAutoFindBestSeparatorTab()
	{
		// Tab-separated values → 3 numeric columns.
		QString text = QStringLiteral("1.0\t2.0\t3.0\n4.0\t5.0\t6.0");
		QCOMPARE(FindBestSeparator(text), QChar('\t'));
	}

	// -----------------------------------------------------------------------
	// Parsing tests via test-friend wrapper of loadCloudFromFormatedAsciiStream
	// -----------------------------------------------------------------------

	void testCommaAsDecimal()
	{
		ccHObject container("test");
		AsciiFilterTestFriend af;

		// "1,5" with commaAsDecimal=true → French locale → 1.5
		QByteArray data = "1,5;2,5;3,5\n";
		int count = ParseWith(af, data, container, MakeXYZSequence(),
		                     ',', true, 1000000, 0);

		QCOMPARE(count, 1);
		auto* cloud = static_cast<ccPointCloud*>(container.getChild(0));
		QVERIFY(cloud != nullptr);
		QCOMPARE(cloud->size(), 1u);

		const CCVector3* p = cloud->getPoint(0);
		QCOMPARE(p->x, 1.5f);   // 1,5 → 1.5
		QCOMPARE(p->y, 2.5f);   // 2,5 → 2.5
		QCOMPARE(p->z, 3.5f);   // 3,5 → 3.5
	}

	void testScientificNotation()
	{
		ccHObject container("test");
		AsciiFilterTestFriend af;

		QByteArray data = "1.23e-4,2.5,3.0\n";
		int count = ParseWith(af, data, container, MakeXYZSequence(),
		                     ',', false, 1000000, 0);

		QCOMPARE(count, 1);
		auto* cloud = static_cast<ccPointCloud*>(container.getChild(0));
		QCOMPARE(cloud->size(), 1u);

		const CCVector3* p = cloud->getPoint(0);
		QCOMPARE(p->x, 0.000123f);  // 1.23e-4
		QCOMPARE(p->y, 2.5f);
		QCOMPARE(p->z, 3.0f);
	}

	void testSkipLines()
	{
		ccHObject container("test");
		AsciiFilterTestFriend af;

		// First 2 lines are header; data starts at line 3.
		QByteArray data = "Header line 1\nMeta,unused,data\n10.0,20.0,30.0\n";
		int count = ParseWith(af, data, container, MakeXYZSequence(),
		                     ',', false, 1000000, 2);  // skipLines=2

		QCOMPARE(count, 1);
		auto* cloud = static_cast<ccPointCloud*>(container.getChild(0));
		QCOMPARE(cloud->size(), 1u);

		const CCVector3* p = cloud->getPoint(0);
		QCOMPARE(p->x, 10.0f);
		QCOMPARE(p->y, 20.0f);
		QCOMPARE(p->z, 30.0f);
	}

	void testMissingColumns()
	{
		ccHObject container("test");
		AsciiFilterTestFriend af;

		// Last row has only 2 columns instead of 3 (XYZ sequence needs 3).
		// The filter should skip the incomplete row and load only the valid row.
		QByteArray data = "1.0,2.0,3.0\n4.0,5.0\n";
		int count = ParseWith(af, data, container, MakeXYZSequence(),
		                     ',', false, 1000000, 0);

		// Only the complete row is loaded
		QCOMPARE(count, 1);
		auto* cloud = static_cast<ccPointCloud*>(container.getChild(0));
		QCOMPARE(cloud->size(), 1u);

		// Verify the valid point was loaded correctly
		const CCVector3* p = cloud->getPoint(0);
		QCOMPARE(p->x, 1.0f);
		QCOMPARE(p->y, 2.0f);
		QCOMPARE(p->z, 3.0f);
	}

	void testChunkSplitting()
	{
		ccHObject container("test");
		AsciiFilterTestFriend af;

		// 5 data rows with maxCloudSize=2 → should split into multiple chunks
		QByteArray data = "1,2,3\n4,5,6\n7,8,9\n10,11,12\n13,14,15\n";
		int count = ParseWith(af, data, container, MakeXYZSequence(),
		                     ',', false, 2, 0);  // maxCloudSize=2

		// At least 2 child clouds from chunk splitting
		QVERIFY2(count > 1, "Chunk splitting should produce more than one cloud");

		// Sum of points across all chunks = 5
		unsigned totalPoints = 0;
		for (int i = 0; i < count; ++i)
		{
			auto* cloud = static_cast<ccPointCloud*>(container.getChild(i));
			totalPoints += cloud->size();
		}
		QCOMPARE(totalPoints, 5u);
	}
};

QTEST_MAIN(TestAsciiFilter)
#include "TestAsciiFilter.moc"
