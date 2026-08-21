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
 * @file qCoreIO.cpp
 *
 * @brief Core I/O plugin implementation
 *
 * The qCoreIO plugin provides built-in I/O formats for CloudCompare:
 *
 * ## Supported Formats
 *
 * | Format | Extension | Type |
 * |-------|----------|------|
 * | STL (Stereolithography) | .stl | Mesh |
 * | OFF (Object File Format) | .off | Mesh |
 * | PTX (Leica Cyclone) | .ptx | Cloud |
 * | MA (Riegl) | .ma | Cloud |
 * | MASCARET | .mascii | Cloud |
 * | PDMS (Aveva) | .exp | Mesh/Cloud |
 * | SimpleBin | .bin | Cloud |
 *
 * ## Plugin Entry
 *
 * Registers all filters via getFilters() and adds them to FileIOFilter.
 *
 * @extends QObject
 * @extends ccIOPluginInterface
 */

#include "qCoreIO.h"

#include "HeightProfileFilter.h"
#include "MAFilter.h"
#include "MascaretFilter.h"
#include "OFFFilter.h"
#include "ObjFilter.h"
#include "PDMSFilter.h"
#include "PTXFilter.h"
#include "STLFilter.h"
#include "SimpleBinFilter.h"
#include "VTKFilter.h"

qCoreIO::qCoreIO(QObject* parent)
    : QObject(parent)
    , ccIOPluginInterface(":/CC/plugin/CoreIO/info.json")
{
}

void qCoreIO::registerCommands(ccCommandLineInterface* inCmdLine)
{
	Q_UNUSED(inCmdLine);
}

ccIOPluginInterface::FilterList qCoreIO::getFilters()
{
	return {
	    FileIOFilter::Shared(new PTXFilter),
	    FileIOFilter::Shared(new SimpleBinFilter),
	    FileIOFilter::Shared(new ObjFilter),
	    FileIOFilter::Shared(new VTKFilter),
	    FileIOFilter::Shared(new STLFilter),
	    FileIOFilter::Shared(new OFFFilter),
	    FileIOFilter::Shared(new PDMSFilter),
	    FileIOFilter::Shared(new MAFilter),
	    FileIOFilter::Shared(new MascaretFilter),
	    FileIOFilter::Shared(new HeightProfileFilter),
	};
}
