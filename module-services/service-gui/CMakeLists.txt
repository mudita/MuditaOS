include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/messages"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/.."
		"${CMAKE_CURRENT_LIST_DIR}/messages"
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ServiceGUI.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerGUI.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/messages/DrawMessage.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ServiceGUI.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WorkerGUI.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/messages/DrawMessage.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/messages/GUIMessage.hpp"
)

