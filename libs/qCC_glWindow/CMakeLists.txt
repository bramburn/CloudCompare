project( QCC_GL_LIB )

add_library( ${PROJECT_NAME} SHARED )

add_subdirectory( include )
add_subdirectory( src )

target_compile_definitions( ${PROJECT_NAME} PRIVATE CCGLWINDOW_LIB_BUILD )

# MSVC + vcpkg toolchain: add Windows SDK lib path for mpr.lib, d3d11.lib, dxgi.lib, d3d12.lib
if( MSVC AND CC_WINDOWS_SDK_LIB_DIR )
	get_filename_component( CC_WINDOWS_SDK_LIB_ABS "${CC_WINDOWS_SDK_LIB_DIR}" ABSOLUTE )
	if( EXISTS "${CC_WINDOWS_SDK_LIB_ABS}" )
		add_link_options( "/LIBPATH:\"${CC_WINDOWS_SDK_LIB_ABS}\"" )
		message( STATUS "qCC_glWindow: added /LIBPATH:${CC_WINDOWS_SDK_LIB_ABS}" )
	endif()
endif()

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
	    QCC_DB_LIB
        Qt6::OpenGLWidgets
    PRIVATE
        CC_FBO_LIB
)

InstallSharedLibrary( TARGET ${PROJECT_NAME} )
