find_package(Qt6 REQUIRED COMPONENTS Test)

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
        qCC_io
        CCPluginAPI
        Qt6::Test
)

target_include_directories(TestFileIOFilter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/qCC_io/include
)

add_test(NAME TestFileIOFilter COMMAND TestFileIOFilter)

# T1-G: Point cloud
add_executable(TestPointCloud
    TestPointCloud.cpp
)

target_link_libraries(TestPointCloud
    PRIVATE
        qCC_db
        CCCoreLib
        CCPluginAPI
        Qt6::Test
)

target_include_directories(TestPointCloud PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/qCC_db/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/qCC_db/extern/CCCoreLib/include
)

add_test(NAME TestPointCloud COMMAND TestPointCloud)

# T1-H: Shifted object (coordinate shift/scale)
add_executable(TestShiftedObject
    TestShiftedObject.cpp
)

target_link_libraries(TestShiftedObject
    PRIVATE
        qCC_db
        CCCoreLib
        CCPluginAPI
        Qt6::Test
)

target_include_directories(TestShiftedObject PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/qCC_db/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/qCC_db/extern/CCCoreLib/include
)

add_test(NAME TestShiftedObject COMMAND TestShiftedObject)
