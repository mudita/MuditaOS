message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

add_subdirectory( parser )
add_subdirectory( endpoints )
add_subdirectory( messages )

target_sources( ${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/ServiceDesktop.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/WorkerDesktop.cpp"
)

add_dependencies(${PROJECT_NAME} version)

# Host target configuration(mainly used for unit testing)
if (${ENABLE_TESTS})
	add_subdirectory(tests)
endif ()
