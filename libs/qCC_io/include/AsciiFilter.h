#pragma once

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
 * @file AsciiFilter.h
 *
 * @brief ASCII / CSV point cloud file filter
 *
 * Reads and writes point clouds from/to plain-text files with
 * configurable column formats.
 *
 * Supported formats: .txt, .asc, .neu, .xyz, .pts, .csv, and more.
 *
 * Column format (auto-detected or specified by user):
 * - X, Y, Z coordinates (required)
 * - Optional: R, G, B (color), Intensity, Classification, Normal components
 * - Optional: per-column scalar fields
 *
 * Auto-detection: the filter can auto-detect column separator (space, comma,
 * semicolon, tab), decimal separator (comma/period), and column layout
 * from the first few lines of the file.
 *
 * The AsciiOpenDlg dialog lets users preview the file, select columns,
 * set separator, skip lines, and configure import options.
 *
 * @see AsciiOpenDlg for the import configuration dialog
 * @see AsciiSaveDlg for the export configuration dialog
 */

#include "AsciiOpenDlg.h"
#include "AsciiSaveDlg.h"
#include "FileIOFilter.h"

#include <QByteArray>
#include <QTextStream>

/**
 * @class AsciiFilter
 *
 * @brief ASCII/CSV point cloud I/O filter
 *
 * Implements FileIOFilter for ASCII point cloud files.
 *
 * Import pipeline:
 * 1. FileIOFilter::LoadFromFile routes to AsciiFilter::loadFile
 * 2. Opens AsciiOpenDlg for column/format selection (first-use or user-requested)
 * 3. Calls loadStream() to parse text
 * 4. Calls loadCloudFromFormatedAsciiStream() with the detected/configured format
 * 5. Returns a ccPointCloud in the container
 *
 * Export pipeline:
 * 1. FileIOFilter::SaveToFile routes to AsciiFilter::saveToFile
 * 2. Opens AsciiSaveDlg for format selection
 * 3. Writes coordinates and optionally colors/SF as delimited text
 *
 * @extends FileIOFilter
 */
class QCC_IO_LIB_API AsciiFilter : public FileIOFilter
{
  public:
	/**
	 * @brief Construct the ASCII filter
	 */
	AsciiFilter();

	/**
	 * @brief Get the Qt file filter string for file dialogs
	 */
	static inline QString GetFileFilter()
	{
		return "ASCII cloud (*.txt *.asc *.neu *.xyz *.pts *.csv)";
	}

	// FileIOFilter overrides

	/**
	 * @brief Load an ASCII point cloud file
	 *
	 * Opens AsciiOpenDlg for format selection on first use,
	 * then delegates to loadStream().
	 *
	 * @param[in] filename File path
	 * @param[out] container Loaded cloud (or group) placed here
	 * @param[in] parameters Load parameters (progress, shift, etc.)
	 * @return CC_FILE_ERROR
	 */
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	/**
	 * @brief Check if this filter can save a given entity type
	 *
	 * @param[in] type Entity class type
	 * @param[out] multiple Whether multiple files may be needed
	 * @param[out] exclusive Whether this filter owns the format exclusively
	 * @return true if clouds can be saved as ASCII
	 */
	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;

	/**
	 * @brief Save an entity to an ASCII file
	 *
	 * Opens AsciiSaveDlg for format selection, then writes
	 * coordinates and optionally colors/SF as delimited text.
	 *
	 * @param[in] entity Entity to save (must be cloud or group)
	 * @param[in] filename Target file path
	 * @param[in] parameters Save parameters
	 * @return CC_FILE_ERROR
	 */
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;

	/**
	 * @brief Load ASCII data from an in-memory buffer
	 *
	 * Useful for processing ASCII data from non-file sources.
	 *
	 * @param[in] data In-memory ASCII data
	 * @param[in] sourceName Display name for error messages\n
	 *                       (e.g. filename or clipboard source)
	 * @param[out] container Loaded cloud placed here
	 * @param[in] parameters Load parameters
	 * @return CC_FILE_ERROR
	 */
	CC_FILE_ERROR loadAsciiData(const QByteArray& data, QString sourceName, ccHObject& container, LoadParameters& parameters);

  public: // Persistent default settings (for dialog pre-filling)
	/**
	 * @brief Set default number of header lines to skip
	 *
	 * @param[in] count Lines to skip before data begins
	 */
	static void SetDefaultSkippedLineCount(int count);

	/**
	 * @brief Set whether to suppress label creation on import
	 *
	 * @param[in] state If true, no 2D labels are created even if present in file
	 */
	static void SetNoLabelCreated(bool state);

	/**
	 * @brief Set default coordinate precision for export
	 *
	 * @param[in] prec Number of decimal places
	 */
	static void SetOutputCoordsPrecision(int prec);

	/**
	 * @brief Set default scalar field precision for export
	 *
	 * @param[in] prec Number of decimal places
	 */
	static void SetOutputSFPrecision(int prec);

	/**
	 * @brief Set default column separator for export
	 *
	 * @param[in] separatorIndex 0=space, 1=comma, 2=semicolon, 3=tab
	 */
	static void SetOutputSeparatorIndex(int separatorIndex);

	/**
	 * @brief Set whether to write SF columns before color in export
	 *
	 * @param[in] state If true, SF columns come before R/G/B columns
	 */
	static void SaveSFBeforeColor(bool state);

	/**
	 * @brief Set whether to write column names in header line
	 *
	 * @param[in] state If true, first line contains column names
	 */
	static void SaveColumnsNamesHeader(bool state);

	/**
	 * @brief Set whether to write point count on first line
	 *
	 * @param[in] state If true, first line is the point count
	 */
	static void SavePointCountHeader(bool state);

  protected:
	/**
	 * @brief Parse an ASCII stream
	 *
	 * Wrapper that auto-detects format and delegates to
	 * loadCloudFromFormatedAsciiStream().
	 *
	 * @param[in] stream Text stream of the file
	 * @param[in] filenameOrTitle File path or display name\n
	 *                            Used for error messages and cloud naming
	 * @param[in] dataSize Approximate file size in bytes (for progress)
	 * @param[out] container Loaded cloud placed here
	 * @param[in] parameters Load parameters
	 * @return CC_FILE_ERROR
	 */
	CC_FILE_ERROR loadStream(QTextStream& stream,
	                         QString filenameOrTitle,
	                         qint64 dataSize,
	                         ccHObject& container,
	                         LoadParameters& parameters);

	/**
	 * @brief Parse an ASCII stream with a known column format
	 *
	 * @param[in] stream Text stream of the file
	 * @param[in] filenameOrTitle File path or display name\n
	 * @param[out] container Loaded cloud placed here\n
	 *                       Must be empty on entry
	 * @param[in] openSequence Column-to-field mapping (X,Y,Z,Color,SF1,...)\n
	 *                         Defines which columns map to which point attribute\n
	 *                         and their data types\n
	 * @param[in] separator Column separator character\n
	 * @param[in] commaAsDecimal true = use comma as decimal separator\n
	 * @param[in] approximateNumberOfLines Estimated line count for progress\n
	 * @param[in] fileSize File size for progress estimation\n
	 * @param[in] maxCloudSize Maximum points to load (0 = unlimited)\n
	 * @param[in] skipLines Number of lines to skip at the start\n
	 * @param[in] quaternionScale Scale for quaternion columns (if present)\n
	 * @param[in] parameters Load parameters\n
	 * @param[in] showLabelsIn2D Whether to create 2D labels\n
	 * @return CC_FILE_ERROR
	 */
	CC_FILE_ERROR loadCloudFromFormatedAsciiStream(QTextStream& stream,
	                                               QString filenameOrTitle,
	                                               ccHObject& container,
	                                               const AsciiOpenDlg::Sequence& openSequence,
	                                               char separator,
	                                               bool commaAsDecimal,
	                                               unsigned approximateNumberOfLines,
	                                               qint64 fileSize,
	                                               unsigned maxCloudSize,
	                                               unsigned skipLines,
	                                               double quaternionScale,
	                                               LoadParameters& parameters,
	                                               bool showLabelsIn2D = false);
};
