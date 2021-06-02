
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
		"${CMAKE_CURRENT_LIST_DIR}/Popups.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/VolumeWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/WindowWithTimer.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/HomeModesWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringConfirmationPopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringNotificationPopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringPhoneModePopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringOffPopup.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/PowerOffWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/PowerOffPresenter.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockedInfoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockedWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockInputWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockChangeInfoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/SimLockInputWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/SimInfoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/SimNotReadyWindow.cpp"

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/Popups.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/VolumeWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WindowWithTimer.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/HomeModesWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringConfirmationPopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringNotificationPopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringPhoneModePopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/PowerOffWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/PowerOffPresenter.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/PopupData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockedInfoWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockedWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockInputWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/PhoneLockChangeInfoWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/SimLockInputWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/SimInfoWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/lock-popups/SimNotReadyWindow.hpp"
)
