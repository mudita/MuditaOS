message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
	add_subdirectory( parser )
	add_subdirectory( endpoints )
	target_sources( ${PROJECT_NAME}
			PRIVATE
			"${CMAKE_CURRENT_LIST_DIR}/ServiceDesktop.cpp"
			"${CMAKE_CURRENT_LIST_DIR}/WorkerDesktop.cpp"
			)
else()
	target_sources( ${PROJECT_NAME}
			PRIVATE
			"${CMAKE_CURRENT_LIST_DIR}/ServiceDesktop.cpp")
endif()
