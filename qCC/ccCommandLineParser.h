/**
 * @file ccCommandLineParser.h
 *
 * @brief Command-line parser for CloudCompare
 *
 * This module handles CloudCompare's command-line interface for batch
 * processing. It allows executing various operations on point clouds
 * and meshes without the GUI.
 *
 * @section Command Line Usage
 *
 * Basic syntax:
 * @code
 * CloudCompare -SILENT -CMD -O input.ply -COMPUTE_GRID -SAVE
 * @endcode
 *
 * Common commands:
 * - -O: Open file
 * - -SAVE: Save current entities
 * - -C_EXPORT_FMT: Set export format
 * - -AUTO_SAVE: Enable auto-save
 * - -STAT_TEST: Statistical test
 * - -FILTER_CLOUD: Filter point cloud
 *
 * @author CloudCompare project
 * @see ccCommandLineInterface for the interface definition
 * @see ccCommandLineCommands for available commands
 */

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
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

// interface
#include "ccCommandLineInterface.h"

// Local
#include "ccPluginManager.h"

class ccProgressDialog;
class QDialog;

/**
 * @brief Command-line parser for CloudCompare batch processing
 *
 * Extends ccCommandLineInterface with concrete implementation
 * for parsing and executing command-line operations.
 */
class ccCommandLineParser : public ccCommandLineInterface
{
  public:
	/**
	 * @brief Parse and execute command-line arguments
	 *
	 * @param[in] arguments List of command-line arguments
	 * @param[in] plugins List of available plugins for extended commands
	 * @return Exit code (0 for success, non-zero for error)
	 *
	 * @see ccCommandLineParser::Parse() for the actual implementation
	 */
	static int Parse(const QStringList& arguments, ccPluginInterfaceList& plugins);

	/**
	 * @brief Destructor
	 */
	~ccCommandLineParser() override;

	// inherited from ccCommandLineInterface
	/**
	 * @brief Generate export filename for an entity
	 * @param[in] entityDesc Entity to export
	 * @param[in] extension File extension (auto-detected if empty)
	 * @param[in] suffix Optional suffix to add to filename
	 * @param[out] baseOutputFilename Base output path (if specified)
	 * @param[in] forceNoTimestamp Don't add timestamp to filename
	 * @return Generated filename
	 */
	QString getExportFilename(const CLEntityDesc& entityDesc,
	                          QString extension = QString(),
	                          QString suffix = QString(),
	                          QString* baseOutputFilename = nullptr,
	                          bool forceNoTimestamp = false) const override;
	/**
	 * @brief Export an entity to file
	 * @param[in,out] entityDesc Entity to export
	 * @param[in] suffix Optional filename suffix
	 * @param[out] baseOutputFilename Base output path
	 * @param[in] options Export options
	 * @return Export filename, or empty string on error
	 */
	QString exportEntity(CLEntityDesc& entityDesc,
	                     const QString& suffix = QString(),
	                     QString* baseOutputFilename = nullptr,
	                     ccCommandLineInterface::ExportOptions options = ExportOption::NoOptions) override;
	/**
	 * @brief Remove clouds from the working list
	 * @param[in] onlyLast If true, remove only the most recently added cloud
	 */
	void removeClouds(bool onlyLast = false) override;
	/**
	 * @brief Remove meshes from the working list
	 * @param[in] onlyLast If true, remove only the most recently added mesh
	 */
	void removeMeshes(bool onlyLast = false) override;
	/**
	 * @brief Select clouds based on criteria
	 * @param[in] options Selection criteria
	 * @return true if selection was successful
	 */
	bool selectClouds(const SelectEntitiesOptions& options) override;
	/**
	 * @brief Select meshes based on criteria
	 * @param[in] options Selection criteria
	 * @return true if selection was successful
	 */
	bool selectMeshes(const SelectEntitiesOptions& options) override;
	QStringList& arguments() override
	{
		return m_arguments;
	}
	/**
	 * @brief Get the command-line arguments (const)
	 * @return Reference to arguments list
	 */
	const QStringList& arguments() const override
	{
		return m_arguments;
	}
	/**
	 * @brief Register a custom command
	 * @param[in] command Command to register
	 * @return true if registration succeeded
	 */
	bool registerCommand(Command::Shared command) override;
	QDialog* widgetParent() override
	{
		return m_parentWidget;
	}
	/**
	 * @brief Print verbose message
	 * @param[in] message Message to print
	 */
	void printVerbose(const QString& message) const override;
	/**
	 * @brief Print info message
	 * @param[in] message Message to print
	 */
	void print(const QString& message) const override;
	/**
	 * @brief Print highlighted message
	 * @param[in] message Message to print
	 */
	void printHigh(const QString& message) const override;
	/**
	 * @brief Print debug message
	 * @param[in] message Message to print
	 */
	void printDebug(const QString& message) const override;
	/**
	 * @brief Print warning message
	 * @param[in] message Warning message
	 */
	void warning(const QString& message) const override;
	/**
	 * @brief Print debug warning message
	 * @param[in] message Warning message
	 */
	void warningDebug(const QString& message) const override;
	/**
	 * @brief Print error message (always returns false)
	 * @param[in] message Error message
	 * @return Always false (for chaining in conditional expressions)
	 */
	bool error(const QString& message) const override;
	/**
	 * @brief Print debug error message (always returns false)
	 * @param[in] message Error message
	 * @return Always false (for chaining in conditional expressions)
	 */
	bool errorDebug(const QString& message) const override;
	bool saveClouds(QString suffix = QString(), bool allAtOnce = false, const QString* allAtOnceFileName = nullptr) override;
	bool saveMeshes(QString suffix = QString(), bool allAtOnce = false, const QString* allAtOnceFileName = nullptr) override;
	bool importFile(QString filename, const GlobalShiftOptions& globalShiftOptions, FileIOFilter::Shared filter = FileIOFilter::Shared(nullptr)) override;
	void setGlobalShiftOptions(const GlobalShiftOptions& globalShiftOptions) override;
	void updateInteralGlobalShift(const GlobalShiftOptions& globalShiftOptions) override;
	QString cloudExportFormat() const override
	{
		return m_cloudExportFormat;
	}
	QString cloudExportExt() const override
	{
		return m_cloudExportExt;
	}
	QString meshExportFormat() const override
	{
		return m_meshExportFormat;
	}
	QString meshExportExt() const override
	{
		return m_meshExportExt;
	}
	QString hierarchyExportFormat() const override
	{
		return m_hierarchyExportFormat;
	}
	QString hierarchyExportExt() const override
	{
		return m_hierarchyExportExt;
	}
	void setCloudExportFormat(QString format, QString ext) override
	{
		m_cloudExportFormat = format;
		m_cloudExportExt = ext;
	}
	void setMeshExportFormat(QString format, QString ext) override
	{
		m_meshExportFormat = format;
		m_meshExportExt = ext;
	}
	void setHierarchyExportFormat(QString format, QString ext) override
	{
		m_hierarchyExportFormat = format;
		m_hierarchyExportExt = ext;
	}

  protected: // other methods
	//! Default constructor
	/** Shouldn't be called by user.
	 **/
	ccCommandLineParser();

	void registerBuiltInCommands();

	void cleanup();

	//! Parses the command line
	int start(QDialog* parent = nullptr);

  private: // members
	//! Current cloud(s) export format (can be modified with the 'COMMAND_CLOUD_EXPORT_FORMAT' option)
	QString m_cloudExportFormat;
	//! Current cloud(s) export extension (warning: can be anything)
	QString m_cloudExportExt;
	//! Current mesh(es) export format (can be modified with the 'COMMAND_MESH_EXPORT_FORMAT' option)
	QString m_meshExportFormat;
	//! Current mesh(es) export extension (warning: can be anything)
	QString m_meshExportExt;
	//! Current hierarchy(ies) export format (can be modified with the 'COMMAND_HIERARCHY_EXPORT_FORMAT' option)
	QString m_hierarchyExportFormat;
	//! Current hierarchy(ies) export extension (warning: can be anything)
	QString m_hierarchyExportExt;

	//! Mesh filename
	QString m_meshFilename;

	//! Arguments
	QStringList m_arguments;

	//! Registered commands
	QMap<QString, Command::Shared> m_commands;

	//! Oprhan entities
	ccHObject m_orphans;

	//! Shared progress dialog
	ccProgressDialog* m_progressDialog;

	//! Widget parent
	QDialog* m_parentWidget;
};
