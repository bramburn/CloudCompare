# CloudCompare experimental plugins
#
# This folder holds throwaway / concept-test plugins used to explore ideas
# before they graduate into either plugins/core/ or get dropped entirely.
# Each subfolder is a self-contained plugin gated by its own CMake option
# (PLUGIN_EXPERIMENTAL_QXXX) so we can mix and match which experiments ship.
#
# See README.md in this folder for the rules and workflow.

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/qHelloCloud )
