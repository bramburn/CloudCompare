option( PLUGIN_STANDARD_QM3C2 "Install qM3C2 plugin" OFF )

if( PLUGIN_STANDARD_QM3C2 )
	project( QM3C2_PLUGIN )

	AddPlugin( NAME ${PROJECT_NAME} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS )

	add_subdirectory( include )
	add_subdirectory( src )
	add_subdirectory( ui )
endif()
