project(service-evtmgr)

set(SOURCES
        EventManager.cpp
        WorkerEventCommon.cpp
        api/EventManagerServiceAPI.cpp
        api/torch.cpp
        backlight-handler/BacklightHandlerCommon.cpp
        battery/BatteryController.cpp
        battery/BatteryController.hpp
        battery-level-check/BatteryLevelCheck.cpp
        battery-brownout-detector/BatteryBrownoutDetector.cpp
        screen-light-control/ControlFunctions.cpp
        screen-light-control/ScreenLightControlParameters.cpp
        vibra/Vibra.cpp
)

add_library(service-evtmgr STATIC)

target_sources(service-evtmgr PRIVATE ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
         ${CMAKE_CURRENT_LIST_DIR}/service-evtmgr/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        log
        module-apps
        module-gui
        module-utils
        service-appmgr
        service-audio
        service-cellular
        service-desktop
        service-bluetooth
        sml::sml
        Microsoft.GSL::GSL
        eventstore
    PUBLIC
        service-db
        sys-service
        sys-manager
        messagetype
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
