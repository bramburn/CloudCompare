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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#pragma once

// Qt
#include <QIcon>
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>

// Qt version
#include <qglobal.h>

class ccExternalFactory;
class ccCommandLineInterface;

/**
 * @brief Plugin type flags
 *
 * Defines the category of functionality a plugin provides.
 * A plugin can have multiple types by combining flags with OR.
 */
enum CC_PLUGIN_TYPE
{
	CC_STD_PLUGIN       = 1, //!< Standard processing plugin (algorithms, tools)
	CC_GL_FILTER_PLUGIN = 2, //!< OpenGL rendering filter/effect
	CC_IO_FILTER_PLUGIN = 4, //!< File I/O handler plugin
};

/**
 * @brief Base interface for all CloudCompare plugins
 *
 * This abstract class defines the contract that all CloudCompare plugins
 * must implement. It provides metadata access (name, description, authors)
 * and lifecycle management (start/stop).
 *
 * Plugins can also provide:
 * - Custom object factories for handling custom entity types
 * - Command-line commands for batch processing
 *
 * @note This is version 3.2 of the plugin interface
 *
 * @par Plugin Types:
 * - Standard plugins (CC_STD_PLUGIN) provide processing algorithms
 * - GL filter plugins (CC_GL_FILTER_PLUGIN) provide post-processing effects
 * - I/O plugins (CC_IO_FILTER_PLUGIN) provide file format support
 *
 * @see ccStdPluginInterface for standard plugin base class
 * @see ccPluginManager for plugin discovery and loading
 */
class ccPluginInterface
{
  public:
	/**
	 * @brief Represents a person associated with the plugin
	 * Used to represent authors, maintainers, or other contributors.
	 */
	struct Contact
	{
		QString name;  //!< Person's name
		QString email; //!< Email address
	};

	//! List of contacts (authors, maintainers, etc.)
	using ContactList = QList<Contact>;

	/**
	 * @brief Reference to documentation about the plugin
	 * Represents a journal article, paper, or online resource.
	 */
	struct Reference
	{
		QString article; //!< Article title or citation
		QString url;     //!< URL to the resource (if online)
	};

	//! List of references
	using ReferenceList = QList<Reference>;

  public:
	/**
	 * @brief Virtual destructor
	 */
	virtual ~ccPluginInterface() = default;

	/**
	 * @brief Get the plugin type
	 * @return Plugin type (CC_STD_PLUGIN, CC_GL_FILTER_PLUGIN, etc.)
	 */
	virtual CC_PLUGIN_TYPE getType() const = 0;

	/**
	 * @brief Check if this is a core plugin
	 * Core plugins are bundled with CloudCompare and cannot be disabled.
	 * @return true if this is a core plugin, false otherwise
	 */
	virtual bool isCore() const = 0;

	/**
	 * @brief Get the plugin's short name
	 * Used for menu entries, plugin lists, and identification.
	 * @return The plugin's short name
	 */
	virtual QString getName() const = 0;

	/**
	 * @brief Get the plugin's description
	 * Provides a longer description for tooltips and help dialogs.
	 * @return The plugin's description
	 */
	virtual QString getDescription() const = 0;

	/**
	 * @brief Get the plugin's icon
	 * Returns an icon to display in the UI (menus, toolbar, etc.).
	 * Return a default/null icon if not needed.
	 * @return The plugin's icon
	 */
	virtual QIcon getIcon() const = 0;

	/**
	 * @brief Get literature references for the plugin
	 * Returns a list of articles, papers, or websites that document
	 * the algorithms or methods used in this plugin.
	 * @return List of references (can be empty)
	 */
	virtual ReferenceList getReferences() const = 0;

	/**
	 * @brief Get the plugin's authors
	 * @return List of author contacts
	 */
	virtual ContactList getAuthors() const = 0;

	/**
	 * @brief Get the plugin's maintainers
	 * @return List of maintainer contacts
	 */
	virtual ContactList getMaintainers() const = 0;

	/**
	 * @brief Start the plugin
	 * Called when the plugin should begin operation.
	 * Initialize any resources, start threads, or register commands here.
	 * @return true if startup succeeded, false otherwise
	 */
	virtual bool start() = 0;

	/**
	 * @brief Stop the plugin
	 * Called when the plugin should clean up and stop operation.
	 * Release resources, stop threads, and unregister commands here.
	 */
	virtual void stop() = 0;

	/**
	 * @brief Get the custom objects factory
	 * If the plugin defines custom entity types, it can provide
	 * a factory for serializing/deserializing them in BIN files.
	 * @return Pointer to a custom factory, or nullptr if not applicable
	 */
	virtual ccExternalFactory* getCustomObjectsFactory() const = 0;

	/**
	 * @brief Register command-line commands
	 * Allows plugins to add custom commands for batch processing.
	 * @param[in,out] cmd The command-line interface for registration
	 * @warning Avoid command names that conflict with core CloudCompare
	 */
	virtual void registerCommands(ccCommandLineInterface* cmd) = 0;

  protected:
	friend class ccPluginManager;

	/**
	 * @brief Set the plugin's IID (internal use)
	 * @param[in] iid The interface identifier from Q_PLUGIN_METADATA
	 */
	virtual void setIID(const QString& iid) = 0;

	/**
	 * @brief Get the plugin's IID (internal use)
	 * @return The plugin's interface identifier
	 */
	virtual const QString& IID() const = 0;
};

Q_DECLARE_METATYPE(const ccPluginInterface*);

Q_DECLARE_INTERFACE(ccPluginInterface, "cccorp.cloudcompare.ccPluginInterface/3.2")
