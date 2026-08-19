# ------------------------------------------------------------------------------
# Qt
# ------------------------------------------------------------------------------

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )

# FIXME Eventually turn this on when we've completed the move to targets
#set( CMAKE_AUTOUIC ON )
find_package( Qt6
    COMPONENTS
        Concurrent
        Core
        Gui
        OpenGL
		OpenGLWidgets
        PrintSupport
        Svg
        Widgets
    REQUIRED
)

# turn on QStringBuilder for more efficient string construction
#	see https://doc.qt.io/qt-6/qstring.html#more-efficient-string-construction
add_definitions( -DQT_USE_QSTRINGBUILDER )
				

# ------------------------------------------------------------------------------
# OpenGL
# ------------------------------------------------------------------------------

if ( UNIX )
	set(OpenGL_GL_PREFERENCE GLVND)
endif()

if ( MSVC )
	# Locate the newest installed Windows SDK (10.x) — the "um/x64" folder
	# contains d3d11.lib, dxgi.lib, d3d12.lib, mpr.lib etc. used by Qt6 OpenGL + Direct3D.
	# We store this in a global variable so targets can append
	# /LIBPATH:"..." to the linker's search path without disturbing the defaults.
	#
	# "Program Files (x86)" has spaces+parens which break file(GLOB) on CMake 3.x.
	# Strategy: write a temp batch script and execute it — avoids CMake's shell escaping issues.
	# The batch file uses the long path directly (no glob).
	set( _CC_WINSDK_BATCH "${CMAKE_BINARY_DIR}/cc_winsdk_list.bat" )
	file( WRITE "${_CC_WINSDK_BATCH}"
		"@echo off\r\n"
		"dir /b \"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\*\"\r\n"
	)
	execute_process(
		COMMAND cmd /c "${_CC_WINSDK_BATCH}"
		OUTPUT_VARIABLE _CC_WINSDK_CMDOUT
		ERROR_VARIABLE _CC_WINSDK_CMDERR
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_STRIP_TRAILING_WHITESPACE
	)
	file( REMOVE "${_CC_WINSDK_BATCH}" )
	if( _CC_WINSDK_CMDOUT AND NOT _CC_WINSDK_CMDERR )
		# Parse the output (one version per line)
		string( REGEX REPLACE "\r?\n" ";" _CC_WINSDK_VERSIONS "${_CC_WINSDK_CMDOUT}" )
		list( SORT _CC_WINSDK_VERSIONS )
		list( REVERSE _CC_WINSDK_VERSIONS )
		list( GET _CC_WINSDK_VERSIONS 0 _CC_WINSDK_VER )
		set( CC_WINDOWS_SDK_LIB_DIR
			"C:/Program Files (x86)/Windows Kits/10/Lib/${_CC_WINSDK_VER}/um/x64" )
		if( EXISTS "${CC_WINDOWS_SDK_LIB_DIR}" )
			message( STATUS "Windows SDK lib dir: ${CC_WINDOWS_SDK_LIB_DIR}" )
		else()
			unset( CC_WINDOWS_SDK_LIB_DIR )
			message( WARNING "Windows SDK version ${_CC_WINSDK_VER} found but um/x64 subfolder missing" )
		endif()
	else()
		message( WARNING "Could not enumerate Windows SDK directory (dir failed)" )
	endif()
	unset( _CC_WINSDK_BATCH )
	unset( _CC_WINSDK_CMDOUT )
	unset( _CC_WINSDK_CMDERR )
	unset( _CC_WINSDK_VERSIONS )
	unset( _CC_WINSDK_VER )
endif()
				
# ------------------------------------------------------------------------------
# OpenMP
# ------------------------------------------------------------------------------

if ( NOT APPLE )
	find_package(OpenMP QUIET)
	if (OPENMP_FOUND)
		message(STATUS "OpenMP found")
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	endif()
endif()

