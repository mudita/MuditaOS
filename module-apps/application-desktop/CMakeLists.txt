
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
		"${CMAKE_CURRENT_LIST_DIR}/widgets/DesktopInputWidget.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DeadBatteryWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/LogoWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/ChargingBatteryWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Reboot.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/Update.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateProgress.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PostUpdateWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPullWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPushWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiInternalMsgWindow.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/ActiveNotificationsModel.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/ApplicationDesktop.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/DesktopData.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/DesktopStyle.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/data/Mmi.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/widgets/DesktopInputWidget.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/DesktopMainWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MenuWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Reboot.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/Update.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/UpdateProgress.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/PostUpdateWindow.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/Names.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPullWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiPushWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/windows/MmiInternalMsgWindow.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/models/ActiveNotificationsModel.hpp"
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
