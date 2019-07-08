
include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationViewer.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ViewWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationViewer.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ViewWindow.hpp"
)

