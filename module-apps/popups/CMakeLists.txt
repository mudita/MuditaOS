
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
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedInfoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/LockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockBaseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinLock.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinLockHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PukLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/ScreenLockBaseBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/ScreenLockBox.cpp"
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
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedInfoWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PhoneLockedWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/LockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockBaseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-windows/PinLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinHash.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinLock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinLockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PinLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/PukLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/ScreenLockBaseBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/ScreenLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-widgets/SimLockBox.hpp"
)
