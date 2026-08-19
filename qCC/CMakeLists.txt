project( CloudCompare VERSION 2.14.0 )

include( CMakePolicies NO_POLICY_SCOPE )

option( CC_USE_EIGEN
	"Compile CloudCompare with Eigen (for registration)"
	OFF
)

# Sentry crash reporting
option( CC_USE_SENTRY
	"Compile with Sentry crash/reporting integration"
	ON
)

if( CC_USE_SENTRY )
	# vcpkg exposes sentry-native as the CMake package "sentry"
	find_package( sentry CONFIG )
	if( sentry_FOUND )
		set( CC_SENTRY_AVAILABLE ON )
		message( STATUS "Sentry: found — CC_SENTRY_AVAILABLE=ON sentry_DIR=${sentry_DIR}" )
	else()
		set( CC_SENTRY_AVAILABLE OFF )
		message( STATUS "sentry-native not found; Sentry disabled. Install with: vcpkg install sentry-native:x64-windows" )
	endif()
endif()

if ( CC_USE_EIGEN )
	set ( EIGEN_ROOT_DIR "" CACHE PATH "Eigen root (contains the Eigen directory)" )
	if ( NOT EIGEN_ROOT_DIR )
		message( SEND_ERROR "No Eigen root directory specified (EIGEN_ROOT_DIR)" )
	endif()
endif()

include_directories( ${CloudComparePlugins_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/db_tree )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ui_templates )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB header_list *.h  )
file( GLOB source_list *.cpp )

file( GLOB ui_list ui_templates/*.ui )
file( GLOB qrc_list *.qrc )

file( GLOB txt_list TODO.txt ${CloudCompareProjects_SOURCE_DIR}/CHANGELOG.md )

add_subdirectory( db_tree )
add_subdirectory( pluginManager )

add_subdirectory( extern/QCustomPlot EXCLUDE_FROM_ALL )

qt6_wrap_ui( generated_ui_list ${ui_list} )
qt6_add_resources( generated_qrc_list ${qrc_list} )

if( MSVC )
	# App icon with MSVC
	set( rc_list images/icon/cc_icon.rc )

	#to get rid of the (system) console
	add_executable( ${PROJECT_NAME} WIN32 ${header_list} ${source_list} ${generated_ui_list} ${generated_qrc_list} ${rc_list} ${txt_list} ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/windows/qt6.natvis )
elseif( APPLE )
	add_executable( ${PROJECT_NAME} MACOSX_BUNDLE ${header_list} ${source_list} ${generated_ui_list} ${generated_qrc_list} ${txt_list} )
else()
	add_executable( ${PROJECT_NAME} ${header_list} ${source_list} ${generated_ui_list} ${generated_qrc_list} ${rc_list} ${txt_list} )
endif()

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		CCAppCommon
		QCustomPlot
		Qt6::PrintSupport
		Qt6::Widgets
		Qt6::OpenGLWidgets
		Qt6::OpenGL
)

if( CC_SENTRY_AVAILABLE )
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			sentry::sentry
	)
	# Pass hardcoded DSN and release string directly.
	# @VAR@ substitution does not work in target_compile_definitions;
	# hardcode here to avoid a configure_file() dependency.
	target_compile_definitions( ${PROJECT_NAME}
		PRIVATE
			CC_SENTRY_DSN="https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184"
			CC_SENTRY_RELEASE="cloudcompare@2.14.0"
	)
endif()

if( CCCORELIB_USE_QT_CONCURRENT )
	# We want *deployqt to handle this dependency, so instead of just copying the file
	# link to the target.
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			Qt6::Concurrent
	)
endif()

# Eigen
if ( CC_USE_EIGEN )

	target_include_directories( ${PROJECT_NAME}
		PUBLIC
			${EIGEN_ROOT_DIR}
	)

	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			CC_USES_EIGEN
	)

endif()

# Add custom preprocessor definitions
if( WIN32 )
	if (MSVC)
		set_target_properties( ${PROJECT_NAME} PROPERTIES LINK_FLAGS " /MANIFEST:NO" )
	endif()
endif()

# App icon with Code::Blocks/MinGW
if( WIN32 )
	if( MINGW )
		if (NOT CMAKE_RC_COMPILER)
			set( CMAKE_RC_COMPILER windres )
		endif()
		add_custom_command( TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_RC_COMPILER} -i ${CMAKE_CURRENT_SOURCE_DIR}/images/icon/cc_icon.rc --input-format=rc -o ${CMAKE_CURRENT_BINARY_DIR}/cc_icon.res -O coff )
	endif()
endif()

if (NOT APPLE)
	# Install program.
	# On macOs, the DeployQt step will install the bundle that contains the executable with
	# library paths properly set, reinstalling the executable here would break the bundle.
	install_ext( TARGETS ${PROJECT_NAME} ${CLOUDCOMPARE_DEST_FOLDER} "" )
endif()

# Auxiliary files
set( auxFiles  ${CloudCompareProjects_SOURCE_DIR}/CHANGELOG.md bin_other/license.txt bin_other/global_shift_list_template.txt )

if( WIN32 )
	# Additional auxiliary file(s)
	file( GLOB batch_list bin_other/*.bat )
	list( APPEND auxFiles ${batch_list} )
endif()

# Install auxiliary files
if( WIN32 OR APPLE )
	foreach( filename ${auxFiles} )
		install_ext( FILES ${filename} ${CLOUDCOMPARE_DEST_FOLDER} "" )
	endforeach()
else( UNIX )
	foreach( filename ${auxFiles} )
		install_ext( FILES ${filename} share/cloudcompare "" )
	endforeach()
endif()

# Put together our Application bundle on macOS
if( APPLE )    
    add_subdirectory( Mac )
endif()

# Testing
if ( BUILD_TESTING )
	add_subdirectory( test )
endif()

# Translation
add_subdirectory(translations)

## Shaders
# Copy shader dirs into our shadow build directory
file( COPY ${CC_FBO_LIB_SOURCE_DIR}/shaders DESTINATION "${CMAKE_BINARY_DIR}" )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION "${CMAKE_BINARY_DIR}" )

# Export common shader files to all install destinations
if( APPLE )	
	install( FILES ${CC_FBO_LIB_SOURCE_DIR}/shaders/Bilateral/bilateral.frag DESTINATION ${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders/Bilateral )
	install( FILES ${CC_FBO_LIB_SOURCE_DIR}/shaders/Bilateral/bilateral.vert DESTINATION ${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders/Bilateral )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ColorRamp/color_ramp.frag DESTINATION ${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders/ColorRamp )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.vs DESTINATION ${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders/DrawNormals )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.gs DESTINATION ${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders/DrawNormals )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.fs DESTINATION ${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders/DrawNormals )
elseif( UNIX )
	install( FILES ${CC_FBO_LIB_SOURCE_DIR}/shaders/Bilateral/bilateral.frag DESTINATION share/cloudcompare/shaders/Bilateral )
	install( FILES ${CC_FBO_LIB_SOURCE_DIR}/shaders/Bilateral/bilateral.vert DESTINATION share/cloudcompare/shaders/Bilateral )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ColorRamp/color_ramp.frag DESTINATION share/cloudcompare/shaders/ColorRamp )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.vs DESTINATION share/cloudcompare/shaders/DrawNormals )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.gs DESTINATION share/cloudcompare/shaders/DrawNormals )
	install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.fs DESTINATION share/cloudcompare/shaders/DrawNormals )

else()
	install_ext( FILES ${CC_FBO_LIB_SOURCE_DIR}/shaders/Bilateral/bilateral.frag ${CLOUDCOMPARE_DEST_FOLDER} /shaders/Bilateral )
	install_ext( FILES ${CC_FBO_LIB_SOURCE_DIR}/shaders/Bilateral/bilateral.vert ${CLOUDCOMPARE_DEST_FOLDER} /shaders/Bilateral )
	install_ext( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ColorRamp/color_ramp.frag ${CLOUDCOMPARE_DEST_FOLDER} /shaders/ColorRamp )
	install_ext( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.vs ${CLOUDCOMPARE_DEST_FOLDER} /shaders/DrawNormals )
	install_ext( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.gs ${CLOUDCOMPARE_DEST_FOLDER} /shaders/DrawNormals )
	install_ext( FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/DrawNormals/DrawNormals.fs ${CLOUDCOMPARE_DEST_FOLDER} /shaders/DrawNormals )
endif()

# Install plugins and shaders in the correct folder for each platform
if( APPLE )	
	InstallPlugins(
		DEST_PATH "${CLOUDCOMPARE_MAC_PLUGIN_DIR}"
		SHADER_DEST_PATH "${CLOUDCOMPARE_MAC_BASE_DIR}/Contents/Shaders"
	)
elseif( UNIX )
	InstallPlugins(
		DEST_PATH "${CMAKE_INSTALL_LIBDIR}/cloudcompare/plugins"
		SHADER_DEST_PATH share/cloudcompare/shaders
	)
else()
	InstallPlugins(
		DEST_PATH "${CLOUDCOMPARE_DEST_FOLDER}"
		DEST_FOLDER "plugins"
		SHADER_DEST_PATH "${CLOUDCOMPARE_DEST_FOLDER}"
		SHADER_DEST_FOLDER "shaders"
	)
endif()

if( WIN32 )
	DeployQt(
		TARGET
			${PROJECT_NAME}
		DEPLOY_PATH
			${CLOUDCOMPARE_DEST_FOLDER}
	)

	# After windeployqt, copy sentry-native DLL into the deploy bundle so crashes
	# during app startup are captured.  DeployQt puts the .exe at:
	#   ${CMAKE_CURRENT_BINARY_DIR}/deployqt/CloudCompare.exe
	# (single-config) or ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/deployqt/CloudCompare.exe
	# (multi-config).
	if( CC_SENTRY_AVAILABLE )
		# DeployQt puts the app at <BINARY_DIR>/deployqt/CloudCompare.exe (single-config)
		# or <BINARY_DIR>/<CONFIG>/deployqt/CloudCompare.exe (multi-config).
		if( CMAKE_CONFIGURATION_TYPES )
			set( sentry_deploy_dir "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/deployqt" )
		else()
			set( sentry_deploy_dir "${CMAKE_CURRENT_BINARY_DIR}/deployqt" )
		endif()
		# sentry_DIR is a cache variable set by find_package(sentry CONFIG) above.
		# It points to <vcpkg>/installed/x64-windows/share/sentry/; go up two levels.
		get_filename_component( SENTRY_VCPKG_PREFIX "$CACHE{sentry_DIR}/../.." ABSOLUTE )
		set( SENTRY_DLL "${SENTRY_VCPKG_PREFIX}/bin/sentry.dll" )
		if( EXISTS "${SENTRY_DLL}" )
			add_custom_command(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"${SENTRY_DLL}"
					"${sentry_deploy_dir}"
				COMMENT "Copying sentry.dll into deploy bundle"
				VERBATIM
			)
		else()
			message( WARNING "Sentry DLL not found at ${SENTRY_DLL}" )
		endif()
	endif()

	# CC core DLLs: resolve deployqt path (same logic as above)
	if( CMAKE_CONFIGURATION_TYPES )
		set( cc_deploy_dir "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/deployqt" )
	else()
		set( cc_deploy_dir "${CMAKE_CURRENT_BINARY_DIR}/deployqt" )
	endif()

	# Copy CC core DLLs so the app launches from deployqt/ without a separate
	# script.  These are the DLLs CloudCompare.exe links against directly or
	# transitively; without them the OS shows "DLL not found" before main()
	# runs — which Sentry cannot capture.
	set( CC_CORE_DLLS
		"${CMAKE_BINARY_DIR}/libs/qCC_db/extern/CCCoreLib/CCCoreLib.dll"   # CCCoreLib
		"${CMAKE_BINARY_DIR}/libs/CCAppCommon/CCAppCommon.dll"              # CCAppCommon
		"${CMAKE_BINARY_DIR}/libs/CCPluginAPI/CCPluginAPI.dll"                # CCPluginAPI
		"${CMAKE_BINARY_DIR}/libs/qCC_db/QCC_DB_LIB.dll"                    # QCC_DB_LIB
		"${CMAKE_BINARY_DIR}/libs/qCC_io/QCC_IO_LIB.dll"                    # QCC_IO_LIB
		"${CMAKE_BINARY_DIR}/libs/qCC_glWindow/QCC_GL_LIB.dll"               # QCC_GL_LIB
		"${CMAKE_BINARY_DIR}/libs/CCFbo/CC_FBO_LIB.dll"                     # CC_FBO_LIB
		# hidapi.dll: 3DConnexion 3D mouse support (linked by CCAppCommon)
		"${CMAKE_BINARY_DIR}/libs/CCAppCommon/devices/3dConnexion/extern/hidapi/src/windows/hidapi.dll"
	)
	foreach( dll ${CC_CORE_DLLS} )
		if( EXISTS "${dll}" )
			add_custom_command(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"${dll}"
					"${cc_deploy_dir}"
				COMMENT "Copying CC core DLL into deploy bundle: ${dll}"
				VERBATIM
			)
		endif()
	endforeach()
else()
	DeployQt(
		TARGET
			${PROJECT_NAME}
		DEPLOY_PATH
			${PROJECT_NAME}
	)
endif()

# Add a 'fake' custom target to force the recompilation of ccApplication.ccp in order to
# always get an up-to-date version number (based on the current date)
add_custom_target(CC_UPDATE_VERSION ALL COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/ccApplication.cpp)
