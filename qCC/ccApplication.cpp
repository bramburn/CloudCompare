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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccApplication.cpp
 *
 * @brief CloudCompare Qt Application implementation.
 *
 * @details Implements CloudCompare-specific application initialization,
 * version management, and platform-specific event handling.
 *
 * @see ccApplication
 * @see ccApplicationBase
 */

#include <QtGlobal>

#ifdef Q_OS_MAC
#include <QFileOpenEvent>
#endif

// qCC_io
#include "FileIO.h"
#include "ccApplication.h"
#include "mainwindow.h"

/**
 * @brief Maps file format versions to minimum required CloudCompare versions.
 *
 * @details This static lookup table maps internal file format version numbers
 * to the earliest CloudCompare version capable of reading files
 * saved with that format version.
 *
 * @note File format versions (10, 20, 30, etc.) are incremented
 *       whenever the file format structure changes, independent of
 *       CloudCompare release versions.
 *
 * @see ccApplication::GetMinCCVersionForFileVersion()
 */
struct FileVersionToCCVersion : QMap<short, QString>
{
	/**
	 * @brief Initialize the version mapping table.
	 *
	 * Populates the map with known file version to CloudCompare version
	 * correspondence. Versions are added chronologically.
	 */
	FileVersionToCCVersion()
	{
		insert(10, "1.0 (before 2012)");
		insert(20, "2.0 (05/04/2012)");
		insert(21, "2.4 (07/02/2012)");
		insert(22, "2.4 (11/26/2012)");
		insert(23, "2.4 (02/07/2013)");
		insert(24, "2.4 (02/22/2013)");
		insert(25, "2.4 (03/16/2013)");
		insert(26, "2.4 (04/03/2013)");
		insert(27, "2.4 (04/12/2013)");
		insert(28, "2.5.0 (07/12/2013)");
		insert(29, "2.5.0 (08/14/2013)");
		insert(30, "2.5.0 (08/30/2013)");
		insert(31, "2.5.0 (09/25/2013)");
		insert(32, "2.5.1 (10/11/2013)");
		insert(33, "2.5.2 (12/19/2013)");
		insert(34, "2.5.3 (01/09/2014)");
		insert(35, "2.5.4 (02/13/2014)");
		insert(36, "2.5.5 (05/30/2014)");
		insert(37, "2.5.5 (08/24/2014)");
		insert(38, "2.6.0 (09/14/2014)");
		insert(39, "2.6.1 (01/30/2015)");
		insert(40, "2.6.2 (08/06/2015)");
		insert(41, "2.6.2 (09/01/2015)");
		insert(42, "2.6.2 (10/07/2015)");
		insert(43, "2.6.3 (01/07/2016)");
		insert(44, "2.8.0 (07/07/2016)");
		insert(45, "2.8.0 (10/06/2016)");
		insert(46, "2.8.0 (11/03/2016)");
		insert(47, "2.9.0 (12/22/2016)");
		insert(48, "2.10.0 (10/19/2018)");
		insert(49, "2.11.0 (03/31/2019)");
		insert(50, "2.11.0 (10/06/2019)");
		insert(51, "2.12.0 (03/29/2019)");
		insert(52, "2.12.0 (11/30/2020)");
		insert(53, "2.13.alpha (10/02/2022)");
		insert(54, "2.13.alpha (01/29/2023)");
	}

	/**
	 * @brief Look up the minimum CloudCompare version for a file version.
	 *
	 * @param[in] fileVersion The internal file format version number.
	 * @return QString describing the minimum required CloudCompare version,
	 *         or "Unknown version" if the version is not in the table.
	 */
	QString getMinCCVersion(short fileVersion) const
	{
		if (contains(fileVersion))
		{
			return value(fileVersion);
		}
		else
		{
			return "Unknown version";
		}
	}
};

//! Static instance of the version mapping table
static FileVersionToCCVersion s_fileVersionToCCVersion;

/**
 * @brief Get minimum CloudCompare version for a file format version.
 *
 * @param[in] fileVersion Internal file format version number.
 * @return QString describing the minimum required CloudCompare version.
 *
 * @see GetMinCCVersionForFileVersion() in header.
 */
QString ccApplication::GetMinCCVersionForFileVersion(short fileVersion)
{
	return s_fileVersionToCCVersion.getMinCCVersion(fileVersion);
}

/**
 * @brief Construct the CloudCompare application.
 *
 * @param[in] argc Reference to argument count from main().
 * @param[in] argv Array of argument strings from main().
 * @param[in] isCommandLine True if running in command-line (non-GUI) mode.
 */
ccApplication::ccApplication(int& argc, char** argv, bool isCommandLine)
    : ccApplicationBase(argc, argv, isCommandLine, QString("2.14.beta (%1)").arg(__DATE__))
{
	setApplicationName("CloudCompare");

	FileIO::setWriterInfo(applicationName(), versionStr());
}

/**
 * @brief Handle application-level events.
 *
 * @param[in] inEvent The event to process.
 * @return true if the event was handled, false otherwise.
 *
 * @details Processes special Qt events, particularly:
 * - QEvent::FileOpen: macOS file association handling
 *
 * On macOS, when CloudCompare is set as the default application
 * for a file type and a file is double-clicked in Finder,
 * Qt sends a QEvent::FileOpen event. This handler processes
 * the event by loading the file into the main window.
 */
bool ccApplication::event(QEvent* inEvent)
{
#ifdef Q_OS_MAC
	switch (inEvent->type())
	{
	case QEvent::FileOpen:
	{
		MainWindow* mainWindow = MainWindow::TheInstance();

		if (mainWindow == nullptr)
		{
			return false;
		}

		mainWindow->addToDB(QStringList(static_cast<QFileOpenEvent*>(inEvent)->file()));
		return true;
	}

	default:
		break;
	}
#endif

	return ccApplicationBase::event(inEvent);
}
