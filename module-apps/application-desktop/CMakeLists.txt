
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
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinLockHandler.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/NotificationsBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/DesktopInputWidget.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBaseWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ScreenLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SimLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PukLockBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PowerOffWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DeadBatteryWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LogoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ChargingBatteryWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockedInfoWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Reboot.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/Update.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateProgress.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PostUpdateWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPullWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPushWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiInternalMsgWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ScreenLockBaseBox.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/presenter/PowerOffPresenter.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationDesktop.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/LockPhoneData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/Style.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/Mmi.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinHash.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinLock.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/DesktopInputWidget.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/PinLockHandler.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/NotificationsBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBaseWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ScreenLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/SimLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PukLockBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PinLockWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PowerOffWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockedInfoWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Reboot.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/Update.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateProgress.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PostUpdateWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Names.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPullWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPushWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiInternalMsgWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ScreenLockBaseBox.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LockWindow.hpp"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-cellular
        service-db
        service-desktop
        service-time
        service-appmgr
        utils-bootconfig
    )
