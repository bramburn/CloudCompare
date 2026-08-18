#ifndef FILEIO_H
#define FILEIO_H

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
 * @file FileIO.h
 *
 * @brief File I/O metadata
 *
 * Metadata for file I/O operations.
 *
 * @author CloudCompare project
 */

#include "qCC_io.h"

#include <QString>

/**
 * @brief File I/O metadata
 *
 * Stores metadata for file writers.
 */
class FileIO
{
  public:
	/// Deleted default constructor
	FileIO() = delete;

	/// Set writer info
	QCC_IO_LIB_API static void setWriterInfo(const QString& applicationName, const QString& version);

	/// Get writer info string
	QCC_IO_LIB_API static QString writerInfo();

	/// Get application name
	QCC_IO_LIB_API static QString applicationName();

	/// Get version
	QCC_IO_LIB_API static QString version();

	/// Get created by string
	QCC_IO_LIB_API static QString createdBy();

	/// Get created date/time string
	QCC_IO_LIB_API static QString createdDateTime();

  private:
	static QString s_applicationName;
	static QString s_version;
	static QString s_writerInfo;
};
#endif
