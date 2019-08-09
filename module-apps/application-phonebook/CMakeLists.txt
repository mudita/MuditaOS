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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationPhonebook.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookMainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookNewContact.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationPhonebook.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookNewContact.hpp"
)

