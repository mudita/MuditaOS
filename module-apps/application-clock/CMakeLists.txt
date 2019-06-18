
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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationClock.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ClockMainWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationClock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ClockMainWindow.hpp"
)

