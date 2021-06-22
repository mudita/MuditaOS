target_include_directories(apps-common

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_sources(apps-common

	PRIVATE
		HomeModesWindow.cpp
		Popups.cpp
		PowerOffWindow.cpp
		TetheringConfirmationPopup.cpp
		TetheringNotificationPopup.cpp
		TetheringPhoneModePopup.cpp
		TetheringOffPopup.cpp
		VolumeWindow.cpp
		WindowWithTimer.cpp
		lock-popups/PhoneLockedInfoWindow.cpp
		lock-popups/PhoneLockedWindow.cpp
		lock-popups/PhoneLockInputWindow.cpp
		lock-popups/PhoneLockChangeInfoWindow.cpp
		lock-popups/SimLockInputWindow.cpp
		lock-popups/SimInfoWindow.cpp
		lock-popups/SimNotReadyWindow.cpp
		presenter/PowerOffPresenter.cpp

	PRIVATE
		HomeModesWindow.hpp
		Popups.hpp
		PowerOffWindow.hpp
		TetheringConfirmationPopup.hpp
		TetheringNotificationPopup.hpp
		TetheringPhoneModePopup.hpp
		VolumeWindow.hpp
		WindowWithTimer.hpp
		data/PopupData.hpp
		lock-popups/PhoneLockedInfoData.hpp
		lock-popups/PhoneLockedInfoWindow.hpp
		lock-popups/PhoneLockedWindow.hpp
		lock-popups/PhoneLockInputWindow.hpp
		lock-popups/PhoneLockChangeInfoWindow.hpp
		lock-popups/SimLockInputWindow.hpp
		lock-popups/SimInfoWindow.hpp
		lock-popups/SimNotReadyWindow.hpp
		presenter/PowerOffPresenter.hpp
)
