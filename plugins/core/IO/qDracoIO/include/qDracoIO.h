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
// #                    COPYRIGHT: CloudCompare project                     #
// #                                                                        #
// ##########################################################################

/**
 * @file qDracoIO.h
 *
 * @brief Draco I/O plugin
 *
 * Plugin for Google Draco compressed mesh I/O.
 *
 * @author CloudCompare project
 */

#include <ccIOPluginInterface.h>

/**
 * @brief Draco I/O plugin
 *
 * Read/write Google Draco compressed meshes.
 */
class qDracoIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qDracoIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qDracoIO(QObject* parent = nullptr);

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* cmd) override;

	/// Get list of supported filters
	FilterList getFilters() override;
};
