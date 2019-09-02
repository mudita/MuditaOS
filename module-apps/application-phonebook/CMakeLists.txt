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
        "${CMAKE_CURRENT_LIST_DIR}/ContactBuilder.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/ContactInputBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookMainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookNewContact.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookOptions.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookInfoWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/ContactsVBox.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationPhonebook.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/ContactBuilder.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/ContactInputBox.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookNewContact.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookOptions.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookInfoWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/ContactsVBox.hpp"
)

