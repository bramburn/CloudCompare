option( PLUGIN_STANDARD_QCSF "Install qCSF plugin" OFF )

if( PLUGIN_STANDARD_QCSF )
	project( QCSF_PLUGIN )

	AddPlugin( NAME ${PROJECT_NAME} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS )

	# Algorithm library — STATIC so tests can link without going through the MODULE.
	# Defined here (not in src/) so CMP0079 allows linking to ${PROJECT_NAME}.
	add_library( CSF_lib STATIC
		${CMAKE_CURRENT_SOURCE_DIR}/src/Cloth.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Cloud2CloudDist.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CSF.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Particle.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Rasterization.cpp
	)
	target_compile_definitions( CSF_lib PRIVATE CSF_LIB_EXPORTS )
	target_include_directories( CSF_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
	# CSF.cpp / Cloth.cpp / Rasterization.cpp need CC and Qt headers
	target_link_libraries( CSF_lib
		PUBLIC    CCCoreLib
		PRIVATE   CCPluginAPI CCPluginStub
		PRIVATE   Qt6::Core Qt6::Widgets
	)

	# Link MODULE to the algorithm library (plain sig to match AddPlugin)
	target_link_libraries( ${PROJECT_NAME} CSF_lib )

	add_subdirectory( include )
	add_subdirectory( src )
	add_subdirectory( ui )
endif()
