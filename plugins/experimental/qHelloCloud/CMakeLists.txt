# CloudCompare experimental plugin: qHelloCloud
#
# Smoke test: a hello-world action and a load-and-display action.
# Use this as the copy-paste template for new experiments under plugins/experimental/.

option( PLUGIN_EXPERIMENTAL_QHELLOCLOUD "Install experimental qHelloCloud plugin (hello + load point cloud)" OFF )

if ( PLUGIN_EXPERIMENTAL_QHELLOCLOUD )
	project( qHelloCloud )

	AddPlugin( NAME ${PROJECT_NAME} )

	add_subdirectory( include )
	add_subdirectory( src )

	# No external dependencies; CCPluginAPI + CCCoreLib + CCPluginStub are
	# added by AddPlugin() automatically.
endif()
