include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/core"
		"${CMAKE_CURRENT_LIST_DIR}/widgets"
)

target_include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/core"
		"${CMAKE_CURRENT_LIST_DIR}/widgets"
)

target_sources(${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/Common.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/Common.hpp"
)

