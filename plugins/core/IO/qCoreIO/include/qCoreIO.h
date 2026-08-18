#ifndef QCORE_IO_HEADER
#define QCORE_IO_HEADER

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
 * @file qCoreIO.h
 *
 * @brief Core I/O plugin
 *
 * Core file I/O plugin for CloudCompare.
 *
 * @author CloudCompare project
 */

#include <ccIOPluginInterface.h>

/**
 * @brief Core I/O plugin
 *
 * Core file I/O handlers (OBJ, PLY, BIN, etc.)
 */
class qCoreIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qCoreIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qCoreIO(QObject* parent = nullptr);

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* inCmdLine) override;

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif // QCORE_IO_HEADER
