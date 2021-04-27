
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

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/Popups.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/VolumeWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/WindowWithTimer.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/HomeModesWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringConfirmationPopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringNotificationPopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/TetheringPhoneModePopup.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/PopupData.hpp"
)
