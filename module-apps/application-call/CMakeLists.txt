
include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/data"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/data"
)

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCall.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EnterNumberWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EmergencyCallWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/Icon.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/AddContactIcon.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationCall.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EnterNumberWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/EmergencyCallWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/CallWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/CallSwitchData.hpp"
)

