
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
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLockHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLockSubject.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/SimLockHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/SimLockSubject.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/LockPolicyHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockInputWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/Lock.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockBoxConstantSize.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockBoxAlternatingSize.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhoneLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/SimLockBox.cpp"

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/data/LockData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockStyle.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/PhoneLockMessages.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/SimLockMessages.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/PhoneLockSubject.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/LockPolicyHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/SimLockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/handlers/SimLockSubject.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockInputWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockHash.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/Lock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockBoxConstantSize.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/LockBoxAlternatingSize.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PhoneLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/SimLockBox.hpp"
)
