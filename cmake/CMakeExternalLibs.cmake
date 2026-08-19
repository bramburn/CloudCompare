# ------------------------------------------------------------------------------
# RC compiler launcher: fix Unix-style -I / -D flags for rc.exe
# ------------------------------------------------------------------------------
# CMake's Ninja generator produces Unix-style -D and -I flags for all compilers,
# including rc.exe.  rc.exe only understands Windows-style /D and /I, so we
# intercept the command via a launcher that converts the flag style.
if( WIN32 AND MSVC )
	# Build the path to the Python wrapper.  Python handles path parsing
	# correctly without Windows shell escaping issues.
	file( TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/rc_wrapper.py" _CC_RC_WRAPPER )
	set( CMAKE_RC_COMPILER_LAUNCHER
		"${_CC_RC_WRAPPER}"
		CACHE FILEPATH "Python launcher: converts -D/-I to /D//I for rc.exe" FORCE )
	mark_as_advanced( CMAKE_RC_COMPILER_LAUNCHER )
	unset( _CC_RC_WRAPPER )
endif()

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
	#
	# "Program Files (x86)" has spaces+parens which break file(GLOB) on CMake 3.x.
	# Strategy: write a temp batch script and execute it — avoids CMake's shell escaping issues.
	# The batch file uses the long path directly (no glob).

	# MSVC library and include paths: needed for CRT libs and headers.
	# VCToolsInstallDir is set by vcvars64.bat and points to the MSVC toolset dir
	# (e.g. "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\").
	if( DEFINED ENV{VCToolsInstallDir} )
		set( _CC_VCTOOLSINSTALLDIR "$ENV{VCToolsInstallDir}" )
		string( REPLACE "\\" "/" _CC_VCTOOLSINSTALLDIR_FWD "${_CC_VCTOOLSINSTALLDIR}" )
		set( CC_MSVC_LIB_DIR "${_CC_VCTOOLSINSTALLDIR_FWD}lib/x64" )
		set( CC_MSVC_INCLUDE_DIR "${_CC_VCTOOLSINSTALLDIR_FWD}include" )
		if( EXISTS "${CC_MSVC_LIB_DIR}" )
			message( STATUS "MSVC lib dir: ${CC_MSVC_LIB_DIR}" )
		else()
			unset( CC_MSVC_LIB_DIR )
		endif()
		if( NOT EXISTS "${CC_MSVC_INCLUDE_DIR}" )
			unset( CC_MSVC_INCLUDE_DIR )
		endif()
		unset( _CC_VCTOOLSINSTALLDIR )
		unset( _CC_VCTOOLSINSTALLDIR_FWD )
	endif()

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
		set( CC_WINDOWS_SDK_INCLUDE_DIR
			"C:/Program Files (x86)/Windows Kits/10/Include/${_CC_WINSDK_VER}/um" )
		set( CC_WINDOWS_SDK_SHARED_INCLUDE_DIR
			"C:/Program Files (x86)/Windows Kits/10/Include/${_CC_WINSDK_VER}/shared" )
		set( CC_WINDOWS_SDK_UCRT_INCLUDE_DIR
			"C:/Program Files (x86)/Windows Kits/10/Include/${_CC_WINSDK_VER}/ucrt" )
		set( CC_WINDOWS_SDK_LIB_DIR
			"C:/Program Files (x86)/Windows Kits/10/Lib/${_CC_WINSDK_VER}/um/x64" )
		set( CC_WINDOWS_SDK_UCRT_LIB_DIR
			"C:/Program Files (x86)/Windows Kits/10/Lib/${_CC_WINSDK_VER}/ucrt/x64" )
		if( EXISTS "${CC_WINDOWS_SDK_LIB_DIR}" )
			message( STATUS "Windows SDK lib dir: ${CC_WINDOWS_SDK_LIB_DIR}" )
			message( STATUS "Windows SDK include dir: ${CC_WINDOWS_SDK_INCLUDE_DIR}" )
			message( STATUS "Windows SDK UCRT lib dir: ${CC_WINDOWS_SDK_UCRT_LIB_DIR}" )
			if( CC_MSVC_LIB_DIR )
				message( STATUS "MSVC lib dir: ${CC_MSVC_LIB_DIR}" )
			endif()
		endif()
		# Build the complete /LIBPATH list: MSVC libs + SDK um libs + SDK ucrt libs.
		# vcpkg prepends its own /LIBPATHs which take precedence, so we need to
		# add the SDK and MSVC paths to CMAKE_EXE/SHARED_LINKER_FLAGS.
		# CMAKE_*_LINKER_FLAGS are already cache vars — FORCE is required to update them.
		set( _CC_LIBPATH "/LIBPATH:\"${CC_WINDOWS_SDK_LIB_DIR}\"" )
		if( CC_MSVC_LIB_DIR )
			set( _CC_LIBPATH "${_CC_LIBPATH} /LIBPATH:\"${CC_MSVC_LIB_DIR}\"" )
		endif()
		if( EXISTS "${CC_WINDOWS_SDK_UCRT_LIB_DIR}" )
			set( _CC_LIBPATH "${_CC_LIBPATH} /LIBPATH:\"${CC_WINDOWS_SDK_UCRT_LIB_DIR}\"" )
		endif()
		set( CMAKE_EXE_LINKER_FLAGS
			"${CMAKE_EXE_LINKER_FLAGS} ${_CC_LIBPATH}"
			CACHE STRING "Additional linker flags (MSVC + SDK lib paths)" FORCE )
		set( CMAKE_SHARED_LINKER_FLAGS
			"${CMAKE_SHARED_LINKER_FLAGS} ${_CC_LIBPATH}"
			CACHE STRING "Additional shared-library linker flags (MSVC + SDK lib paths)" FORCE )
		unset( _CC_LIBPATH )
		# Build include flags for C++ compiler: start from existing CMAKE_CXX_FLAGS, add include paths.
		# We keep /EHsc (C++ exception handling) — the Python wrapper handles RC compilation.
		set( _CC_INCLUDE_FLAGS "${CMAKE_CXX_FLAGS}" )
		if( CC_MSVC_INCLUDE_DIR )
			set( _CC_INCLUDE_FLAGS "${_CC_INCLUDE_FLAGS} /I\"${CC_MSVC_INCLUDE_DIR}\"" )
		endif()
		if( EXISTS "${CC_WINDOWS_SDK_UCRT_INCLUDE_DIR}" )
			set( _CC_INCLUDE_FLAGS "${_CC_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_UCRT_INCLUDE_DIR}\"" )
		endif()
		set( _CC_INCLUDE_FLAGS "${_CC_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_INCLUDE_DIR}\"" )
		if( EXISTS "${CC_WINDOWS_SDK_SHARED_INCLUDE_DIR}" )
			set( _CC_INCLUDE_FLAGS "${_CC_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_SHARED_INCLUDE_DIR}\"" )
		endif()
		set( CMAKE_CXX_FLAGS "${_CC_INCLUDE_FLAGS}" CACHE STRING "C++ flags (MSVC + SDK include paths)" FORCE )
		unset( _CC_INCLUDE_FLAGS )
		# Build include flags for C compiler (shapelib etc.) — add UCRT include path.
		# Also strip /EHsc which doesn't apply to pure C code.
		string( REGEX REPLACE " /EHsc" "" _CC_C_BASE_FLAGS "${CMAKE_C_FLAGS}" )
		set( _CC_C_INCLUDE_FLAGS "${_CC_C_BASE_FLAGS}" )
		if( CC_MSVC_INCLUDE_DIR )
			set( _CC_C_INCLUDE_FLAGS "${_CC_C_INCLUDE_FLAGS} /I\"${CC_MSVC_INCLUDE_DIR}\"" )
		endif()
		if( EXISTS "${CC_WINDOWS_SDK_UCRT_INCLUDE_DIR}" )
			set( _CC_C_INCLUDE_FLAGS "${_CC_C_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_UCRT_INCLUDE_DIR}\"" )
		endif()
		set( _CC_C_INCLUDE_FLAGS "${_CC_C_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_INCLUDE_DIR}\"" )
		if( EXISTS "${CC_WINDOWS_SDK_SHARED_INCLUDE_DIR}" )
			set( _CC_C_INCLUDE_FLAGS "${_CC_C_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_SHARED_INCLUDE_DIR}\"" )
		endif()
		set( CMAKE_C_FLAGS "${_CC_C_INCLUDE_FLAGS}" CACHE STRING "C flags (MSVC + SDK include paths)" FORCE )
		unset( _CC_C_INCLUDE_FLAGS )
		unset( _CC_C_BASE_FLAGS )
		# Build include flags for RC compiler: MSVC + SDK includes ONLY — rc.exe does not
		# understand C++ flags like /EHsc. Must be kept strictly separate from CMAKE_CXX_FLAGS.
		set( _CC_RC_INCLUDE_FLAGS "" )
		if( CC_MSVC_INCLUDE_DIR )
			set( _CC_RC_INCLUDE_FLAGS "${_CC_RC_INCLUDE_FLAGS} /I\"${CC_MSVC_INCLUDE_DIR}\"" )
		endif()
		if( EXISTS "${CC_WINDOWS_SDK_UCRT_INCLUDE_DIR}" )
			set( _CC_RC_INCLUDE_FLAGS "${_CC_RC_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_UCRT_INCLUDE_DIR}\"" )
		endif()
		set( _CC_RC_INCLUDE_FLAGS "${_CC_RC_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_INCLUDE_DIR}\"" )
		if( EXISTS "${CC_WINDOWS_SDK_SHARED_INCLUDE_DIR}" )
			set( _CC_RC_INCLUDE_FLAGS "${_CC_RC_INCLUDE_FLAGS} /I\"${CC_WINDOWS_SDK_SHARED_INCLUDE_DIR}\"" )
		endif()
		if( _CC_RC_INCLUDE_FLAGS )
			# Strip C++-specific flags from any value CMAKE_RC_FLAGS had from the MSVC platform
			# (which prepends CMAKE_CXX_FLAGS including /EHsc).
			string( REGEX REPLACE " /EHsc" "" _CC_RC_BASE_FLAGS "${CMAKE_RC_FLAGS}" )
			string( REGEX REPLACE " /GR" "" _CC_RC_BASE_FLAGS "${_CC_RC_BASE_FLAGS}" )
			set( CMAKE_RC_FLAGS
				"${_CC_RC_BASE_FLAGS} ${_CC_RC_INCLUDE_FLAGS}"
				CACHE STRING "Resource compiler flags (MSVC + SDK, no C++ flags)" FORCE )
			unset( _CC_RC_BASE_FLAGS )
		endif()
		unset( _CC_RC_INCLUDE_FLAGS )
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
