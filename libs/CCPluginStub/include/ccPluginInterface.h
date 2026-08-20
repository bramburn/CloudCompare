// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.     #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccPluginInterface.h
 *
 * @brief Base interface for all CloudCompare plugins.
 *
 * @details Defines the contract that all CloudCompare plugins must implement.
 * Provides metadata access and lifecycle management.
 *
 * ## Plugin Types
 *
 * - **Standard plugins (CC_STD_PLUGIN)**: Processing algorithms and tools
 * - **GL filter plugins (CC_GL_FILTER_PLUGIN)**: Post-processing effects
 * - **I/O plugins (CC_IO_FILTER_PLUGIN)**: File format support
 *
 * ## Lifecycle
 *
 * 1. Plugin discovered via QPluginLoader
 * 2. `start()` called to initialize
 * 3. Plugin registers actions, menus, commands
 * 4. `stop()` called on shutdown
 *
 * ## Usage
 *
 * @code
 * class MyPlugin : public QObject, public ccStdPluginInterface {
 *     Q_OBJECT
 *     Q_PLUGIN_METADATA(IID "..." FILE "info.json")
 *     Q_INTERFACES(ccPluginInterface)
 * public:
 *     CC_PLUGIN_TYPE getType() const override { return CC_STD_PLUGIN; }
 *     QString getName() const override { return "My Plugin"; }
 *     bool start() override { return true; }
 *     void stop() override {}
 *     QList<QAction*> getActions() override { return {}; }
 * };
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccStdPluginInterface for standard plugin base
 * @see ccPluginManager for plugin loading
 */

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
 * @brief Plugin type flags.
 *
 * Defines the category of functionality a plugin provides.
 * Combine flags with OR for multiple types.
 */
enum CC_PLUGIN_TYPE
{
	CC_STD_PLUGIN       = 1, //!< Standard processing plugin.
	CC_GL_FILTER_PLUGIN = 2, //!< OpenGL rendering filter.
	CC_IO_FILTER_PLUGIN = 4  //!< File I/O handler plugin.
};

/**
 * @brief Base interface for all CloudCompare plugins.
 *
 * Defines the contract for CloudCompare plugins including
 * metadata access and lifecycle management.
 *
 * @note Interface version: 3.2
 */
class ccPluginInterface
{
  public:
	/**
	 * @brief Contact information.
	 *
	 * Represents a person associated with the plugin.
	 */
	struct Contact
	{
		QString name;  //!< Person's name.
		QString email; //!< Email address.
	};

	//! List of contacts.
	using ContactList = QList<Contact>;

	/**
	 * @brief Literature reference.
	 *
	 * Represents a documentation reference.
	 */
	struct Reference
	{
		QString article; //!< Article title or citation.
		QString url;     //!< URL if online.
	};

	//! List of references.
	using ReferenceList = QList<Reference>;

  public:
	/**
	 * @brief Virtual destructor.
	 */
	virtual ~ccPluginInterface() = default;

	/**
	 * @brief Get plugin type.
	 *
	 * @return Plugin type flags.
	 */
	virtual CC_PLUGIN_TYPE getType() const = 0;

	/**
	 * @brief Check if core plugin.
	 *
	 * @return true if bundled and cannot be disabled.
	 */
	virtual bool isCore() const = 0;

	/**
	 * @brief Get plugin name.
	 *
	 * @return Short name for menus and lists.
	 */
	virtual QString getName() const = 0;

	/**
	 * @brief Get plugin description.
	 *
	 * @return Long description for tooltips.
	 */
	virtual QString getDescription() const = 0;

	/**
	 * @brief Get plugin icon.
	 *
	 * @return Icon for UI display.
	 */
	virtual QIcon getIcon() const = 0;

	/**
	 * @brief Get literature references.
	 *
	 * @return List of references (can be empty).
	 */
	virtual ReferenceList getReferences() const = 0;

	/**
	 @brief Get authors.
	 *
	 * @return List of author contacts.
	 */
	virtual ContactList getAuthors() const = 0;

	/**
	 * @brief Get maintainers.
	 *
	 * @return List of maintainer contacts.
	 */
	virtual ContactList getMaintainers() const = 0;

	/**
	 * @brief Start the plugin.
	 *
	 * @return true if startup succeeded.
	 *
	 * @note Initialize resources and register commands here.
	 */
	virtual bool start() = 0;

	/**
	 * @brief Stop the plugin.
	 *
	 * @note Release resources and unregister commands here.
	 */
	virtual void stop() = 0;

	/**
	 * @brief Get custom objects factory.
	 *
	 * @return Factory or nullptr.
	 *
	 * @note For custom entity types.
	 */
	virtual ccExternalFactory* getCustomObjectsFactory() const = 0;

	/**
	 * @brief Register CLI commands.
	 *
	 * @param[in,out] cmd Command-line interface.
	 *
	 * @warning Avoid conflicting with core commands.
	 */
	virtual void registerCommands(ccCommandLineInterface* cmd) = 0;

  protected:
	friend class ccPluginManager;

	/**
	 * @brief Set IID (internal).
	 *
	 * @param[in] iid Interface identifier.
	 */
	virtual void setIID(const QString& iid) = 0;

	/**
	 * @brief Get IID (internal).
	 *
	 * @return Interface identifier.
	 */
	virtual const QString& IID() const = 0;
};

Q_DECLARE_METATYPE(const ccPluginInterface*);

Q_DECLARE_INTERFACE(ccPluginInterface, "cccorp.cloudcompare.ccPluginInterface/3.2")
