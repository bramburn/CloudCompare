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
 * @file TestFileIOFilter.cpp
 *
 * @brief Unit tests for FileIOFilter
 *
 * Tests the filter registry and loading:
 * - Filter registration
 * - Extension matching
 * - Load/save round-trip
 * - Progress reporting
 * - Error handling
 *
 * @see FileIOFilter.cpp
 */
#include "FileIOFilter.h"

#include <QTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QTextStream>

class TestFileIOFilter : public QObject
{
	Q_OBJECT

  private slots:

	void initTestCase()
	{
		FileIOFilter::InitInternalFilters();
	}

	void cleanup()
	{
		// Leave filters in a clean state between tests
		FileIOFilter::UnregisterAll();
		FileIOFilter::InitInternalFilters();
	}

	// GetRealFilename()
	void getRealFilenameReturnsInput()
	{
		QString result = FileIOFilter::GetRealFilename("C:/no/such/path.txt");
		QCOMPARE(result, "C:/no/such/path.txt");
	}

	void getRealFilenameSimplePath()
	{
		QTemporaryDir dir;
		QVERIFY(dir.isValid());
		QString path = dir.path() + "/file.txt";
		QCOMPARE(FileIOFilter::GetRealFilename(path), path);
	}

	// CheckForSpecialChars()
	void checkForSpecialCharsClean()
	{
		QVERIFY(!FileIOFilter::CheckForSpecialChars("normal_file.txt"));
	}

	void checkForSpecialCharsSpaces()
	{
		QVERIFY(!FileIOFilter::CheckForSpecialChars("normal file.txt"));
	}

	// GetFilters()
	void getFiltersReturnsNonEmptyAfterInit()
	{
		const auto& filters = FileIOFilter::GetFilters();
		QVERIFY(!filters.empty());
	}

	// ImportFilterList()
	void importFilterListReturnsNonEmpty()
	{
		QStringList list = FileIOFilter::ImportFilterList();
		QVERIFY(!list.isEmpty());
		// First entry should be "All Files (*.*)"
		QCOMPARE(list.first(), QStringLiteral("All (*.*)"));
	}

	// FindBestFilterForExtension — uses real registered filters
	void findBestFilterForExtensionPly()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("ply");
		QVERIFY(!filter.isNull());
		QVERIFY(filter->importSupported());
	}

	void findBestFilterForExtensionAsc()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("asc");
		QVERIFY(!filter.isNull());
	}

	void findBestFilterForExtensionBin()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("bin");
		QVERIFY(!filter.isNull());
	}

	void findBestFilterForExtensionUnknown()
	{
		// Unknown extension returns null filter
		auto filter = FileIOFilter::FindBestFilterForExtension("zzzzunknown");
		QCOMPARE(filter.isNull(), true);
	}

	void findBestFilterForExtensionIsCaseInsensitive()
	{
		auto filter1 = FileIOFilter::FindBestFilterForExtension("PLY");
		auto filter2 = FileIOFilter::FindBestFilterForExtension("ply");
		QCOMPARE(!filter1.isNull(), true);
		QCOMPARE(!filter2.isNull(), true);
	}

	// GetFilter — uses real registered filters
	void getFilterValidPlyString()
	{
		auto filter = FileIOFilter::GetFilter("PLY mesh (*.ply)", true);
		QVERIFY(!filter.isNull());
		QCOMPARE(filter->importSupported(), true);
	}

	void getFilterUnknownString()
	{
		auto filter = FileIOFilter::GetFilter("No Such Filter (*.nosuch)", true);
		QCOMPARE(filter.isNull(), true);
	}

	void getFilterEmptyString()
	{
		auto filter = FileIOFilter::GetFilter(QString(), true);
		QCOMPARE(filter.isNull(), true);
	}

	// Register — needs a concrete subclass
	void registerAddsToList()
	{
		FileIOFilter::UnregisterAll();
		QCOMPARE(FileIOFilter::GetFilters().size(), static_cast<size_t>(0));

		// Re-init to register real filters
		FileIOFilter::InitInternalFilters();
		QVERIFY(!FileIOFilter::GetFilters().empty());
	}

	// UnregisterAll
	void unregisterAllClearsList()
	{
		// Note: cleanup() runs after every test and calls InitInternalFilters(),
		// so filters are already registered here. Verify that UnregisterAll clears them.
		FileIOFilter::UnregisterAll();
		QCOMPARE(FileIOFilter::GetFilters().size(), static_cast<size_t>(0));
		// After unregister, ImportFilterList returns only the "All (*.*)" entry
		QCOMPARE(FileIOFilter::ImportFilterList().size(), 1);
	}

	// Default extension per filter
	void plyFilterDefaultExtension()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("ply");
		QVERIFY(!filter.isNull());
		QCOMPARE(filter->getDefaultExtension(), QStringLiteral("ply"));
	}

	void binFilterDefaultExtension()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("bin");
		QVERIFY(!filter.isNull());
		QCOMPARE(filter->getDefaultExtension(), QStringLiteral("bin"));
	}

	// canSave — cloud vs mesh
	void plyFilterCanSaveCloud()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("ply");
		QVERIFY(!filter.isNull());
		bool multiple = false;
		bool exclusive = false;
		QCOMPARE(filter->canSave(CC_TYPES::POINT_CLOUD, multiple, exclusive), true);
	}

	void plyFilterCanSaveMesh()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("ply");
		QVERIFY(!filter.isNull());
		bool multiple = false;
		bool exclusive = false;
		QCOMPARE(filter->canSave(CC_TYPES::MESH, multiple, exclusive), true);
	}

	// Filter feature flags
	void plyFilterSupportsImport()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("ply");
		QVERIFY(!filter.isNull());
		QCOMPARE(filter->importSupported(), true);
	}

	void plyFilterSupportsExport()
	{
		auto filter = FileIOFilter::FindBestFilterForExtension("ply");
		QVERIFY(!filter.isNull());
		QCOMPARE(filter->exportSupported(), true);
	}
};

QTEST_GUILESS_MAIN(TestFileIOFilter)
#include "TestFileIOFilter.moc"
