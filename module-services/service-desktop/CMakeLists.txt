message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

add_subdirectory( parser )
add_subdirectory( endpoints )
target_sources( ${PROJECT_NAME}
		PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ServiceDesktop.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerDesktop.cpp"
		)
