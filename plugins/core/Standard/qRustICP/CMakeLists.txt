# qRustICP — Rust ICP via cc-rust subprocess
#
# Standard CloudCompare plugin that runs the pure-Rust ICP from
# cc-rust/ on two selected point clouds. Implementation: spawns
# `cc_rust_cli.exe icp` as a subprocess, parses the JSON
# transform, and applies it to the data cloud.
#
# Future work: replace the subprocess with a direct CXX FFI
# reverse-direction call (cc-rust/lib.rs has the forward
# direction via the `cxx_ffi` feature; the reverse direction
# via `extern "Rust"` blocks is the obvious next step for a
# sub-second call overhead). For the current scope — proving
# the end-to-end integration works in the real CloudCompare UI
# — the subprocess approach is sufficient.
option( PLUGIN_STANDARD_QRUSTICP "Install qRustICP plugin" OFF )

if( PLUGIN_STANDARD_QRUSTICP )
	project( QRUSTICP_PLUGIN )

	AddPlugin( NAME ${PROJECT_NAME} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS )

	# qRustICP links against CCCoreLib (for ccPointCloud, ccHObject,
	# ccMainAppInterface), CCPluginAPI + CCPluginStub (for the plugin
	# interface), and Qt (for the action + dialog scaffolding). The
	# cc-rust library is a Rust staticlib that we link via
	# cc-rust/target/release/cc_rust.lib (built by `cargo build
	# --release` in cc-rust/). The Rust subprocess path doesn't
	# require the staticlib at all — we just exec `cc_rust_cli.exe`
	# and parse the output. The staticlib link is for the future
	# in-process CXX FFI path; left enabled by default to make the
	# upgrade path straightforward.
	#
	# Note: this must use the same form (plain or keyword) as the
	# AddPlugin macro at plugins/cmake/Plugins.cmake:95 to avoid
	# CMake's "plain signature has already been used" error.
	target_link_libraries( ${PROJECT_NAME}
		CCCoreLib
		CCPluginAPI
		CCPluginStub
		Qt6::Core
		Qt6::Widgets
	)

	# Link cc-rust's Rust staticlib (built once via `cargo build
	# --release` in cc-rust/). The staticlib is in
	# cc-rust/target/release/cc_rust.lib on Windows. Disable the
	# link by setting CC_RUST_LIB="" if you want the pure-subprocess
	# path and don't have the staticlib built.
	if( EXISTS "${CMAKE_SOURCE_DIR}/../../../cc-rust/target/release/cc_rust.lib" )
		target_link_libraries( ${PROJECT_NAME}
			"${CMAKE_SOURCE_DIR}/../../../cc-rust/target/release/cc_rust.lib"
		)
		message( STATUS "qRustICP: linking cc-rust staticlib (in-process FFI path enabled)" )
	else()
		message( STATUS "qRustICP: cc-rust staticlib not found; will use subprocess path only" )
	endif()

	add_subdirectory( include )
	add_subdirectory( src )
endif()
