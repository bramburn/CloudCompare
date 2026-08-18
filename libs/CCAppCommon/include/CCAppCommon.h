// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
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

#pragma once

/**
 * @file CCAppCommon.h
 *
 * @brief App Common library export definitions
 *
 * Defines the CCAPPCOMMON_LIB_API macro for library export/import.
 */

#include <QtCore/QtGlobal>

#if defined(CCAPPCOMMON_LIBRARY_BUILD)
#define CCAPPCOMMON_LIB_API Q_DECL_EXPORT
#else
#define CCAPPCOMMON_LIB_API Q_DECL_IMPORT
#endif
