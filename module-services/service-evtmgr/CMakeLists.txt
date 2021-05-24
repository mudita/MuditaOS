project(service-evtmgr)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
        EventManager.cpp
        WorkerEvent.cpp
        api/EventManagerServiceAPI.cpp
        battery-level-check/BatteryLevelCheck.cpp
        backlight-handler/BacklightHandler.cpp
        screen-light-control/ControlFunctions.cpp
        screen-light-control/ScreenLightControl.cpp
        vibra/Vibra.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-audio
        service-appmgr
        service-cellular
        service-desktop
        service-gui
        module-apps
        module-cellular
        module-gui
        module-utils

)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
