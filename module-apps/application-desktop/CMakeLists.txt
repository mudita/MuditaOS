
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
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationDesktop.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PowerOffWindow.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationDesktop.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PowerOffWindow.hpp"
)

