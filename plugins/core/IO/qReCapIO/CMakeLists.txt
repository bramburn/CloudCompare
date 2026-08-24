
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

option( PLUGIN_IO_QRECAP
	"Install qReCapIO plugin to read RCS (ReCap Station) and RCP (ReCap Photo) files via Autodesk ReCap SDK v27"
	OFF
)

if( PLUGIN_IO_QRECAP )

	# -------------------------------------------------------------------------
	# SDK location — points at the locally-installed ReCap SDK.
	# Do NOT publish SDK binaries to the repository.
	# -------------------------------------------------------------------------
	if( NOT DEFINED RECAP_SDK_ROOT )
		set( RECAP_SDK_ROOT "C:/ReCapSDK_v27.0.3" )
	endif()

	if( NOT EXISTS "${RECAP_SDK_ROOT}/Include" )
		message( FATAL_ERROR
			"ReCap SDK not found at ${RECAP_SDK_ROOT}. "
			"Download from https://developer.autodesk.com/en/docs/recap/v1/overview/ "
			"and set RECAP_SDK_ROOT to the install directory."
		)
	endif()

	# -------------------------------------------------------------------------
	# Find all required .lib files and create IMPORTED targets for each.
	# On Windows, .lib files for DLLs are "import libraries" — CMake's
	# add_library(... IMPORTED) + set_target_properties( IMPORTED_LOCATION )
	# is the correct pattern so target_link_libraries() accepts them.
	# -------------------------------------------------------------------------
	set( RECAP_LIB_NAMES
		AdskRcCommon
		AdskRcCompressUtils
		AdskRcData
		AdskRcDispatch
		AdskRcFileSystemUtility
		AdskRcFilter
		AdskRcFoundation
		AdskRcImport
		# AdskRcManaged excluded: native SDK (v27+) no longer requires it.
		# Keeping it linked would reintroduce the ijwhost.dll dependency.
		AdskRcProjectIO
		AdskRcSpatialReference
		AdskRcStringUtils
		acge
	)

	set( RECAP_FOUND_LIBS "" )
	foreach( LIB ${RECAP_LIB_NAMES} )
		find_library( RECAP_LIB_${LIB}
			NAMES ${LIB}
			HINTS "${RECAP_SDK_ROOT}/Lib" NO_DEFAULT_PATH
		)
		if( RECAP_LIB_${LIB} )
			# Create one IMPORTED SHARED library target per .lib
			add_library( ReCap_${LIB} SHARED IMPORTED GLOBAL )
			set_target_properties( ReCap_${LIB} PROPERTIES
				IMPORTED_IMPLIB "${RECAP_LIB_${LIB}}"
			)
			list( APPEND RECAP_FOUND_LIBS "ReCap_${LIB}" )
		else()
			message( WARNING "ReCap library '${LIB}' not found in ${RECAP_SDK_ROOT}/Lib" )
		endif()
	endforeach()

	if( NOT RECAP_FOUND_LIBS )
		message( FATAL_ERROR "No ReCap libraries found. Cannot build qReCapIO plugin." )
	endif()

	# -------------------------------------------------------------------------
	# Plugin target — pass ReCap libs via DEPENDS so they land in the
	# same target_link_libraries() call as CCCoreLib/CCPluginAPI/CCPluginStub
	# (all-keyword signature).  Do NOT call target_link_libraries again.
	# -------------------------------------------------------------------------
	project( QRECAP_IO_PLUGIN )

	AddPlugin( NAME ${PROJECT_NAME} TYPE io DEPENDS ${RECAP_FOUND_LIBS} )

	# gelib headers use GE_NO_ACAD to avoid requiring the proprietary
	# adesk.h (ObjectARX/RealDWG).  The standalone path provides equivalent
	# typedefs and macros.
	target_compile_definitions( ${PROJECT_NAME} PRIVATE GE_NO_ACAD )

	# Enable AUTOMOC so the Q_OBJECT macro in qReCapIO.h generates
	# the required moc_qReCapIO.cpp (metaObject, qt_metacast, qt_metacall).
	set_target_properties( ${PROJECT_NAME} PROPERTIES AUTOMOC ON )

	# Compile qReCapIO.qrc and embed the resulting .rcc binary in the plugin DLL.
	# qt_add_resources() with CMAKE_AUTORCC ON automatically adds the generated .cpp to
	# the target sources, but we add it explicitly to be safe.
	qt_add_resources( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/qReCapIO.qrc )

	# Must be called AFTER AddPlugin so ${PROJECT_NAME} is a defined target.
	add_subdirectory( include )
	add_subdirectory( src )

	# -------------------------------------------------------------------------
	# Copy ReCap DLLs to the plugin's build output directory so the CMake
	# install (which copies only ${CMAKE_CURRENT_BINARY_DIR}/*.dll) includes
	# them alongside QRECAP_IO_PLUGIN.dll.  Without this, the SDK DLLs end up
	# in a $<CONFIG>/ subdirectory and are silently omitted by the install.
	#
	# v27 layout: SDK DLLs live in Bin/ and Bin/dependencies/ (TBB, xerces).
	# GLOB_RECURSE picks up both locations automatically.
	# -------------------------------------------------------------------------
	set( RECAP_DLL_SRC_DIR "${RECAP_SDK_ROOT}/Bin" )
	set( RECAP_DLL_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
	file( GLOB_RECURSE RECAP_DLLS "${RECAP_DLL_SRC_DIR}/*.dll" )

	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMENT "Copying ReCap SDK DLLs to plugin output directory"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RECAP_DLLS} "${RECAP_DLL_DEST_DIR}"
		VERBATIM
	)

	# Install ReCap SDK DLLs alongside the plugin DLL in the plugin install
	# directory.  The default plugin install copies only *.dll from the plugin's
	# binary dir — this ensures the SDK DLLs are included.
	foreach( RECAP_DLL ${RECAP_DLLS} )
		get_filename_component( RECAP_DLL_NAME "${RECAP_DLL}" NAME )
		install( FILES "${RECAP_DLL}"
			DESTINATION "${CMAKE_INSTALL_PREFIX}/CloudCompare/plugins"
		)
	endforeach()

endif()
