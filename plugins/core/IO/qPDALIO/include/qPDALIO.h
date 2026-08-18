#ifndef PDAL_IO_HEADER
#define PDAL_IO_HEADER

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
 * @file qPDALIO.h
 *
 * @brief PDAL I/O plugin
 *
 * Plugin for PDAL-based file I/O.
 *
 * @author CloudCompare project
 */

#include <ccIOPluginInterface.h>

/**
 * @brief PDAL I/O plugin
 *
 * Read/write files using PDAL library.
 */
class qPDALIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qPDALIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qPDALIO(QObject* parent = nullptr);

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* cmd) override;

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif
