// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
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
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        #
// ##########################################################################

/**
 * @file LasPlugin.cpp
 *
 * @brief LAS/LAZ I/O plugin entry point
 *
 * Registers the LasIOFilter with CloudCompare's file I/O system.
 * Handles QMetaType registration for LasVlr QVariant serialization.
 *
 * ## Plugin Registration
 *
 * The plugin registers LasVlr as a Qt metatype so it can be stored
 * in QVariant (used for round-trip VLR persistence in ccPointCloud metadata).
 *
 * @see LasIOFilter for the main filter implementation
 */

#include "LasPlugin.h"

#include "LasIOFilter.h"
#include "LasVlr.h"

/**
 * @brief Construct the LAS/LAZ plugin
 *
 * Registers the LasVlr metatype for QVariant serialization.
 */
LasPlugin::LasPlugin(QObject* parent)
    : QObject(parent)
    , ccIOPluginInterface(":/CC/plugin/LAS-IO/info.json")
{
	qRegisterMetaType<LasVlr>();

	QMetaType::registerConverter(&LasVlr::toString);
}

/**
 * @brief Get the list of file I/O filters
 *
 * @return Single-element list containing the LasIOFilter
 */
ccIOPluginInterface::FilterList LasPlugin::getFilters()
{
	return {
	    FileIOFilter::Shared(new LasIOFilter),
	};
}
