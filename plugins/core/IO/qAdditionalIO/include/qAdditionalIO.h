#ifndef Q_ADDITIONAL_IO_PLUGIN_HEADER
#define Q_ADDITIONAL_IO_PLUGIN_HEADER

// ##########################################################################
// #                                                                        #
// #         CLOUDCOMPARE PLUGIN: qAdditionalIO                             #
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

/**
 * @file qAdditionalIO.h
 *
 * @brief Additional I/O formats plugin
 *
 * Plugin for additional file format support.
 *
 * @author CloudCompare project
 */

#include <ccIOPluginInterface.h>

/**
 * @brief Additional I/O formats plugin
 *
 * Provides additional file I/O formats.
 */
class qAdditionalIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qAdditionalIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qAdditionalIO(QObject* parent = nullptr);

	/// Destructor
	~qAdditionalIO() override = default;

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* cmd) override;

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif
