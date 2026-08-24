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
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

#pragma once

/**
 * @file ccDefaultPluginInterface.h
 *
 * @brief Default plugin interface base class
 *
 * Base class for CloudCompare plugins with default
 * implementations for common plugin methods.
 *
 * @author CloudCompare project
 */

#include "ccPluginInterface.h"

#include <QString>

class ccDefaultPluginData;

/**
 * @brief Default plugin interface
 *
 * Base class for plugins with default implementations.
 */
class ccDefaultPluginInterface : public ccPluginInterface
{
  public:
	/**
	 * @brief Destructor
	 */
	~ccDefaultPluginInterface() override;

	/// Check if this is a core plugin
	bool isCore() const override;

	/// Get plugin name
	QString getName() const override;

	/// Get plugin description
	QString getDescription() const override;

	/// Get plugin icon
	QIcon getIcon() const override;

	/// Get references
	ReferenceList getReferences() const override;

	/// Get authors
	ContactList getAuthors() const override;

	/// Get maintainers
	ContactList getMaintainers() const override;

	/// Start the plugin
	bool start() override
	{
		return true;
	}

	/// Stop the plugin
	void stop() override
	{
	}

	/// Get custom objects factory
	ccExternalFactory* getCustomObjectsFactory() const override
	{
		return nullptr;
	}

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* cmd) override
	{
		Q_UNUSED(cmd);
	}

  protected:
	/**
	 * @brief Create plugin
	 * @param[in] resourcePath Path to resources
	 */
	ccDefaultPluginInterface(const QString& resourcePath = QString());

  private:
	/// Set plugin IID
	void setIID(const QString& iid) override;

	/// Get plugin IID
	const QString& IID() const override;

	/// Plugin data
	ccDefaultPluginData* m_data;
};
