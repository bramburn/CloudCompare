#ifndef Q_RDB_IO_PLUGIN_HEADER
#define Q_RDB_IO_PLUGIN_HEADER

// ##########################################################################
// #                                                                        #
// #                       CLOUDCOMPARE PLUGIN: qRDBIO                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: RIEGL Laser Measurement Systems GmbH               #
// #                                                                        #
// ##########################################################################

/**
 * @file qRDBIO.h
 *
 * @brief RDB I/O plugin
 *
 * Plugin for RIEGL RDB file I/O.
 *
 * @author RIEGL Laser Measurement Systems GmbH
 */

#include <ccIOPluginInterface.h>

/**
 * @brief RDB I/O plugin
 *
 * Read RIEGL RDB files.
 */
class qRDBIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qRDBIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qRDBIO(QObject* parent = nullptr);

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif // Q_RDB_IO_PLUGIN_HEADER
