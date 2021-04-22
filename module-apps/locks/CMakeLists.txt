
include_directories( ${CMAKE_PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PinLockHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLocHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLockSubject.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBaseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/Lock.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PukLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhoneLockBaseBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhoneLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/SimLockBox.cpp"

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/data/LockData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockStyle.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockMessages.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PinLockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLocHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLockSubject.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBaseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockHash.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/Lock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PukLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhoneLockBaseBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhoneLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/SimLockBox.hpp"
)
