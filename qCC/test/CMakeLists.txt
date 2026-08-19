find_package(Qt6 REQUIRED COMPONENTS Test)

# On Windows/MSVC, the Qt6 OpenGLWidgets component links d3d11.lib / dxgi.lib
# but the Windows SDK library directory is not on the linker's default search path.
# CMake sets it up via the MSVC toolchain on full Visual Studio installs, but the
# Community edition + vcpkg toolchain needs an explicit /LIBPATH for the SDK.
if(MSVC AND DEFINED CC_WINDOWS_SDK_LIB_DIR AND EXISTS "${CC_WINDOWS_SDK_LIB_DIR}")
	add_link_options( "/LIBPATH:\"${CC_WINDOWS_SDK_LIB_DIR}\"" )
endif()

add_executable(TestArgumentParser
    TestArgumentParser.cpp
)

target_link_libraries(TestArgumentParser
    PRIVATE
        CCPluginAPI
        Qt6::Test
)

add_test(NAME TestArgumentParser COMMAND TestArgumentParser)

# T1-B: FileIOFilter core
add_executable(TestFileIOFilter
    TestFileIOFilter.cpp
)

target_link_libraries(TestFileIOFilter
    PRIVATE
        QCC_IO_LIB
        QCC_DB_LIB
        CCCoreLib
        CCPluginAPI
        Qt6::Test
)

add_test(NAME TestFileIOFilter COMMAND TestFileIOFilter)

# T1-G: Point cloud
add_executable(TestPointCloud
    TestPointCloud.cpp
)

target_link_libraries(TestPointCloud
    PRIVATE
        QCC_DB_LIB
        CCCoreLib
        CCPluginAPI
        Qt6::Test
)

add_test(NAME TestPointCloud COMMAND TestPointCloud)

# T1-H: Shifted object (coordinate shift/scale)
add_executable(TestShiftedObject
    TestShiftedObject.cpp
)

target_link_libraries(TestShiftedObject
    PRIVATE
        QCC_DB_LIB
        CCCoreLib
        CCPluginAPI
        Qt6::Test
)

add_test(NAME TestShiftedObject COMMAND TestShiftedObject)
