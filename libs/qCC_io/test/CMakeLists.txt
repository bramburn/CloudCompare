find_package(Qt6 REQUIRED COMPONENTS Test)

if ( OPTION_USE_SHAPE_LIB )
    add_executable( TestShpFilter )

    target_sources( TestShpFilter
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/TestShpFilter.cpp
            ${CMAKE_CURRENT_LIST_DIR}/TestShpFilter.h
            ${CMAKE_CURRENT_LIST_DIR}/TestData.qrc
    )

    target_link_libraries( TestShpFilter
        PRIVATE
            QCC_IO_LIB
            Qt6::Test
    )

    if ( WIN32 )
        set_target_properties( TestShpFilter PROPERTIES
            WIN32_EXECUTABLE False
        )
    endif()

    add_test( NAME TestShpFilter COMMAND TestShpFilter )
endif()

# T2-D: ccGlobalShiftManager
add_executable(TestGlobalShift
    TestGlobalShift.cpp
)

target_link_libraries(TestGlobalShift
    PRIVATE
        QCC_IO_LIB
        QCC_DB_LIB
        CCCoreLib
        Qt6::Test
)

add_test(NAME TestGlobalShift COMMAND TestGlobalShift)

# T2-E: BinFilter
add_executable(TestBinFilter
    TestBinFilter.cpp
)

target_link_libraries(TestBinFilter
    PRIVATE
        QCC_IO_LIB
        QCC_DB_LIB
        CCCoreLib
        Qt6::Test
)

add_test(NAME TestBinFilter COMMAND TestBinFilter)

# T2-F: AsciiFilter
add_executable(TestAsciiFilter
    TestAsciiFilter.cpp
)

target_link_libraries(TestAsciiFilter
    PRIVATE
        QCC_IO_LIB
        QCC_DB_LIB
        CCCoreLib
        Qt6::Test
)

add_test(NAME TestAsciiFilter COMMAND TestAsciiFilter)


