#ifndef Q_LAS_FWF_IO_PLUGIN_HEADER
#define Q_LAS_FWF_IO_PLUGIN_HEADER

// ##########################################################################
// #                                                                        #
// #                    CLOUDCOMPARE PLUGIN: qLasFWFIO                      #
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
// #                         COPYRIGHT: CNRS / OSUR                         #
// #                                                                        #
// ##########################################################################

/**
 * @file qLASFWFIO.h
 *
 * @brief LAS FWF (Full Waveform) I/O plugin
 *
 * Plugin for LAS Full Waveform data I/O.
 *
 * @author CNRS / OSUR
 */

#include <ccIOPluginInterface.h>

/**
 * @brief LAS FWF I/O plugin
 *
 * Read LAS Full Waveform data.
 */
class qLASFWFIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qLAS_FWF_IO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qLASFWFIO(QObject* parent = nullptr);

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* cmd) override;

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif // Q_LAS_FWF_IO_PLUGIN_HEADER
