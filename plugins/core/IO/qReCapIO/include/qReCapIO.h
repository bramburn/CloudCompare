// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.     #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// ##########################################################################

#pragma once

#include "ccIOPluginInterface.h"
#include <QObject>

/**
 * @file qReCapIO.h
 *
 * @brief ReCap I/O plugin.
 *
 * @details Autodesk ReCap RCS/RCP file format support.
 *
 * @author Autodesk / ReCap SDK
 */

/**
 * @brief qReCapIO plugin
 *
 * Provides I/O support for Autodesk ReCap RCS (Station) and RCP (Photo) files
 * via the Autodesk ReCap SDK v26.
 *
 * Read-only: RCS point clouds are loaded as ccPointCloud with colour,
 * intensity, and normals when available.  RCP project files load all
 * constituent scans as separate point clouds.
 *
 * Dependencies: Autodesk ReCap SDK (locally installed; binaries are not
 * published in the repository).
 */
class qReCapIO : public QObject, public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qReCapIO" FILE "../info.json")

  public:
	explicit qReCapIO(QObject* parent = nullptr);
	~qReCapIO() override = default;

	// ccIOPluginInterface
	FilterList getFilters() override;
};
