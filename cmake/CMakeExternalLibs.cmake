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
	# contains d3d11.lib, dxgi.lib, d3d12.lib etc. used by Qt6 OpenGL + Direct3D.
	# We store this in a global variable so test CMakeLists.txt can append
	# /LIBPATH:"..." to the linker's search path without disturbing the defaults.
	file( GLOB _CC_WINSDK_DIRS "C:/Program Files (x86)/Windows Kits/10/Lib/"* )
	if( _CC_WINSDK_DIRS )
		list( SORT _CC_WINSDK_DIRS )
		list( REVERSE _CC_WINSDK_DIRS )
		list( GET _CC_WINSDK_DIRS 0 _CC_WINSDK_VER )
		set( CC_WINDOWS_SDK_LIB_DIR "${_CC_WINSDK_VER}/um/x64" )
		if( EXISTS "${CC_WINDOWS_SDK_LIB_DIR}" )
			message( STATUS "Windows SDK lib dir: ${CC_WINDOWS_SDK_LIB_DIR}" )
		else()
			unset( CC_WINDOWS_SDK_LIB_DIR )
		endif()
	endif()
	unset( _CC_WINSDK_DIRS )
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

