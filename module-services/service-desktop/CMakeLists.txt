message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

add_subdirectory( parser )
add_subdirectory( endpoints )
add_subdirectory( update )

target_sources( ${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/ServiceDesktop.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/WorkerDesktop.cpp"
)

# Host target configuration(mainly used for unit testing)
if (${ENABLE_TESTS})
	add_subdirectory(tests)
endif ()