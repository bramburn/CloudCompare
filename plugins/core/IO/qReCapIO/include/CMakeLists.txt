
# ############################################################################
# #                                                                        #
# #                              CLOUDCOMPARE                              #
# #                                                                        #
# #  This program is free software; you can redistribute it and/or modify  #
# #  it under the terms of the GNU General Public License as published by  #
# #  the Free Software Foundation; version 2 or later of the License.     #
# #                                                                        #
# #  This program is distributed in the hope that it will be useful,       #
# #  but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             #
# #  GNU General Public License for more details.                          #
# #                                                                        #
# ############################################################################

target_include_directories( ${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${RECAP_SDK_ROOT}/Include"
	"${RECAP_SDK_ROOT}/Include/gelib"
)

# qReCapIO.h contains Q_OBJECT and must be listed as a source so CMake's
# AUTOMOC finds it (headers in sibling include/ dirs are not auto-detected).
target_sources( ${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/qReCapIO.h"
)
