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
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhonebookItem.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhonebookListView.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookMainWindow.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookNewContact.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookContact.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookDeleteContact.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookBlockContact.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookSearch.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookOptions.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookOptionsNamecard.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookErrors.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookSearchResults.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/models/PhonebookModel.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/models/SearchResultsModel.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/models/PhonebookModel.hpp"
                "${CMAKE_CURRENT_LIST_DIR}/models/SearchResultsModel.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationPhonebook.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhonebookListView.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhonebookItem.hpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookNewContact.hpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookContact.hpp"
                "${CMAKE_CURRENT_LIST_DIR}/windows/PhonebookOptions.hpp"
)

