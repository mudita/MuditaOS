include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ServiceKbd.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerKbd.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ServiceKbd.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerKbd.hpp"
)

