
# MODULE plugin sources (Qt/CC-specific only).
# CSF_lib is defined in the parent CMakeLists.txt so it can be linked to QCSF_PLUGIN (CMP0079).
target_sources( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/ccCSFDlg.cpp
		${CMAKE_CURRENT_LIST_DIR}/qCSF.cpp
)
