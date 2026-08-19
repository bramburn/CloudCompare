
target_sources( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/ActionHello.h
		${CMAKE_CURRENT_LIST_DIR}/ActionLoadCloud.h
		${CMAKE_CURRENT_LIST_DIR}/qHelloCloud.h
)

target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)
