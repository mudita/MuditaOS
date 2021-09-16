project(service-evtmgr)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
        EventManager.cpp
        WorkerEventCommon.cpp
        api/EventManagerServiceAPI.cpp
        api/torch.cpp
        battery-level-check/BatteryLevelCheck.cpp
        backlight-handler/BacklightHandler.cpp
        battery-brownout-detector/BatteryBrownoutDetector.cpp
        screen-light-control/ControlFunctions.cpp
        screen-light-control/ScreenLightControlParameters.cpp
        vibra/Vibra.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
         ${CMAKE_CURRENT_LIST_DIR}/service-evtmgr/include
)
 

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-apps
        module-gui
        module-utils
        service-appmgr
        service-audio
        service-cellular
        service-desktop
        sml::sml
        Microsoft.GSL::GSL
        eventstore
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
