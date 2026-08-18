#ifndef Q_PHOTOSCAN_IO_PLUGIN_HEADER
#define Q_PHOTOSCAN_IO_PLUGIN_HEADER

// ##########################################################################
// #                                                                        #
// #                   CLOUDCOMPARE PLUGIN: qPhotoScanIO                    #
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
 * @file qPhotoscanIO.h
 *
 * @brief PhotoScan I/O plugin
 *
 * Plugin for Agisoft PhotoScan/Zeply files.
 *
 * @author Daniel Girardeau-Montaut
 */

#include <ccIOPluginInterface.h>

/**
 * @brief PhotoScan I/O plugin
 *
 * Read PhotoScan/Zeply files.
 */
class qPhotoscanIO : public QObject
    , public ccIOPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccPluginInterface ccIOPluginInterface)

	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qPhotoscanIO" FILE "../info.json")

  public:
	/**
	 * @brief Create plugin
	 * @param[in] parent Parent object
	 */
	explicit qPhotoscanIO(QObject* parent = nullptr);

	/// Destructor
	~qPhotoscanIO() override = default;

	/// Get list of supported filters
	FilterList getFilters() override;
};

#endif // Q_PHOTOSCAN_IO_PLUGIN_HEADER
