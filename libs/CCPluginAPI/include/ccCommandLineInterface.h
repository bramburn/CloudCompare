// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        //
// ##########################################################################

/**
 * @file ccCommandLineInterface.h
 *
 * @brief Command line interface for batch processing.
 *
 * @details Defines structures and interface for CloudCompare's
 * command-line batch processing capabilities.
 *
 * ## Overview
 *
 * CloudCompare can run in headless mode to process files
 * without a GUI. This module provides:
 * - Entity descriptions (loaded files)
 * - Command registration
 * - Argument parsing
 * - Batch processing
 *
 * ## Command Structure
 *
 * Commands are registered with:
 * - Name (e.g., "CROP", "RASTER")
 * - Description
 * - Argument parser
 * - Processing function
 *
 * ## Usage
 *
 * @code
 * // Register a command
 * app.addCommand("MY_COMMAND",
 *     "Process cloud",
 *     [](ccCommandLineInterface& cmd) {
 *         // Process
 *         return true;
 *     });
 *
 * // Run from command line
 * CloudCompare -silent -c -my_command input.bin
 * @endcode
 *
 * @author CloudCompare project
 *
 * @see ccCommandLineParser for argument parsing
 */

#pragma once

#include "CCPluginAPI.h"

// qCC_db
#include <ccPointCloud.h>

// qCC_io
#include <FileIOFilter.h>

// Qt
#include <QRegularExpression>
#include <QSharedPointer>
#include <QString>

// System
#include <optional>
#include <vector>

class ccArgumentParser;
class ccGenericMesh;
class ccProgressDialog;

class QDialog;

/// Entity type for command line processing.
enum class CL_ENTITY_TYPE
{
	GROUP, //!< Group entity
	CLOUD, //!< Point cloud
	MESH   //!< Mesh
};

/**
 * @brief Description of a loaded entity.
 *
 * @details Base structure for entities loaded via command line.
 * Stores filename and path information.
 */
struct CCPLUGIN_LIB_API CLEntityDesc
{
	//! Base name (without path)
	QString basename;

	//! Full path
	QString path;

	//! Index if multiple entities in file
	int indexInFile;

	/**
	 * @brief Construct from name.
	 *
	 * @param[in] name Entity name.
	 */
	explicit CLEntityDesc(const QString& name);

	/**
	 * @brief Construct from filename.
	 *
	 * @param[in] filename File name.
	 * @param[in] _indexInFile Entity index.
	 */
	CLEntityDesc(const QString& filename, int _indexInFile);

	/**
	 * @brief Construct from components.
	 *
	 * @param[in] _basename Base name.
	 * @param[in] _path Path.
	 * @param[in] _indexInFile Entity index.
	 */
	CLEntityDesc(const QString& _basename, const QString& _path, int _indexInFile = -1);

	/**
	 * @brief Destructor.
	 */
	virtual ~CLEntityDesc() = default;

	/**
	 * @brief Get the entity.
	 *
	 * @return Loaded entity.
	 */
	virtual ccHObject* getEntity() = 0;

	/**
	 * @brief Get the entity (const).
	 *
	 * @return Loaded entity.
	 */
	virtual const ccHObject* getEntity() const = 0;

	/**
	 * @brief Get entity type.
	 *
	 * @return Entity type.
	 */
	virtual CL_ENTITY_TYPE getCLEntityType() const = 0;
};

/**
 * @brief Description of a loaded group.
 *
 * @extends CLEntityDesc
 */
struct CCPLUGIN_LIB_API CLGroupDesc : CLEntityDesc
{
	//! Group entity
	ccHObject* groupEntity;

	/**
	 * @brief Construct group description.
	 *
	 * @param[in] group Group entity.
	 * @param[in] basename Base name.
	 * @param[in] path File path.
	 */
	CLGroupDesc(ccHObject* group, const QString& basename, const QString& path = QString());

	~CLGroupDesc() override = default;

	ccHObject* getEntity() override;
	const ccHObject* getEntity() const override;
	CL_ENTITY_TYPE getCLEntityType() const override;
};

/**
 * @brief Description of a loaded point cloud.
 *
 * @extends CLEntityDesc
 */
struct CCPLUGIN_LIB_API CLCloudDesc : CLEntityDesc
{
	//! Point cloud
	ccPointCloud* pc;

	//! Default constructor.
	CLCloudDesc();

	/**
	 * @brief Construct cloud description.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] filename File name.
	 * @param[in] index Entity index.
	 */
	CLCloudDesc(ccPointCloud* cloud, const QString& filename = QString(), int index = -1);

	/**
	 * @brief Construct cloud description.
	 *
	 * @param[in] cloud Point cloud.
	 * @param[in] basename Base name.
	 * @param[in] path File path.
	 * @param[in] index Entity index.
	 */
	CLCloudDesc(ccPointCloud* cloud, const QString& basename, const QString& path, int index = -1);

	~CLCloudDesc() override = default;

	ccHObject* getEntity() override;
	const ccHObject* getEntity() const override;
	CL_ENTITY_TYPE getCLEntityType() const override;
};

/**
 * @brief Description of a loaded mesh.
 *
 * @extends CLEntityDesc
 */
struct CCPLUGIN_LIB_API CLMeshDesc : CLEntityDesc
{
	//! Mesh entity
	ccGenericMesh* mesh;

	//! Default constructor.
	CLMeshDesc();

	/**
	 * @brief Construct mesh description.
	 *
	 * @param[in] _mesh Mesh entity.
	 * @param[in] filename File name.
	 * @param[in] index Entity index.
	 */
	CLMeshDesc(ccGenericMesh* _mesh, const QString& filename = QString(), int index = -1);

	/**
	 * @brief Construct mesh description.
	 *
	 * @param[in] _mesh Mesh entity.
	 * @param[in] basename Base name.
	 * @param[in] path File path.
	 * @param[in] index Entity index.
	 */
	CLMeshDesc(ccGenericMesh* _mesh, const QString& basename, const QString& path, int index = -1);

	~CLMeshDesc() override = default;

	ccHObject* getEntity() override;
	const ccHObject* getEntity() const override;
	CL_ENTITY_TYPE getCLEntityType() const override;
};

//! Command line interface
// (continues in implementation file)
