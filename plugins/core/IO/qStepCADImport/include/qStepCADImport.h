#pragma once

// ##########################################################################
// #                                                                        #
// #                 CLOUDCOMPARE PLUGIN: qSTEPCADImport                    #
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
// #                          COPYRIGHT: EDF R&D                            #
// #                                                                        #
// ##########################################################################

/**
 * @file qStepCADImport.h
 *
 * @brief STEP CAD import plugin
 *
 * Plugin for importing STEP CAD files.
 *
 * @author EDF R&D
 */

#include <ccIOPluginInterface.h>

/**
 * @brief STEP CAD import plugin
 *
 * Import STEP CAD files.
 */
class qStepCADImport : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qStepCADImport" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qStepCADImport(QObject* parent = nullptr);

	/// Register command line commands
	void registerCommands(ccCommandLineInterface* inCmdLine) override;

	/// Get list of supported filters
	FilterList getFilters() override;
};
