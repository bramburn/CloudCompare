#pragma once
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

/**
 * @file CCFbo.h
 *
 * @brief Framebuffer Object library export definitions
 *
 * Defines the CCFBO_LIB_API macro for library export/import.
 */

#include <QtCore/QtGlobal>

#if defined(CCFBO_LIBRARY_BUILD)
#define CCFBO_LIB_API Q_DECL_EXPORT
#else
#define CCFBO_LIB_API Q_DECL_IMPORT
#endif
