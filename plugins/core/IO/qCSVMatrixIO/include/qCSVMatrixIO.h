#ifndef Q_CSV_MATRIX_IO_PLUGIN_HEADER
#define Q_CSV_MATRIX_IO_PLUGIN_HEADER

// ##########################################################################
// #                                                                        #
// #                  CLOUDCOMPARE PLUGIN: qCSVMatrixIO                     #
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
// #                  COPYRIGHT: Daniel Girardeau-Montaut                   #
// #                                                                        #
// ##########################################################################

/**
 * @file qCSVMatrixIO.h
 *
 * @brief CSV Matrix I/O plugin
 *
 * Plugin for CSV matrix file (2.5D cloud) I/O.
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ccIOPluginInterface.h>

/**
 * @brief CSV Matrix I/O plugin
 *
 * Read/write CSV matrix files (2.5D clouds).
 */
class qCSVMatrixIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qCSVMatrixIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qCSVMatrixIO(QObject* parent = nullptr);

	/// Destructor
	~qCSVMatrixIO() override = default;

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif // Q_CSV_MATRIX_IO_PLUGIN_HEADER
