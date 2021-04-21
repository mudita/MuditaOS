
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
		"${CMAKE_CURRENT_LIST_DIR}/Popups.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/VolumeWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WindowWithTimer.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/HomeModesWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringConfirmationPopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringNotificationPopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringPhoneModePopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/PinLockHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/PhoneLockObserver.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/PhoneLockSubject.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedInfoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneUnLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/LockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockBaseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/Lock.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PukLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PhoneLockBaseBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PhoneLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/SimLockBox.cpp"

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/Popups.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/VolumeWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WindowWithTimer.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/HomeModesWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringConfirmationPopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringNotificationPopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringPhoneModePopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockStyle.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/PopupData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockMessages.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/LockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/PinLockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/PhoneLockObserver.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-handlers/PhoneLockSubject.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedInfoWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneUnLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/LockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockBaseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinHash.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/Lock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PukLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PhoneLockBaseBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PhoneLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/SimLockBox.hpp"
)
