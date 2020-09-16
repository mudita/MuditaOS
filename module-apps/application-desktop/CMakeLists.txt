
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
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinLock.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/ScreenLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/SimLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PukLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBaseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PowerOffWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Reboot.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationDesktop.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinLock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/ScreenLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/SimLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PukLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBaseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PowerOffWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Reboot.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Names.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockPhoneData.hpp"
)

