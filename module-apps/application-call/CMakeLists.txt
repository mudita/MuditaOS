
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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCall.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EnterNumberWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCall.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EnterNumberWindow.hpp"
)

