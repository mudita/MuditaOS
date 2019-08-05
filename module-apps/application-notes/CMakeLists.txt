
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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationNotes.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesMainWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationNotes.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/NotesMainWindow.hpp"
)

