add_library(evtmgr STATIC)

target_sources(evtmgr
    PRIVATE
        EventManager.cpp
        WorkerEvent.cpp
        WorkerEvent.hpp
        screen-light-control/ScreenLightControl.cpp
        backlight-handler/BacklightHandler.cpp
        user-activity-handler/UserActivityHandler.cpp
    PUBLIC
        include/evtmgr/EVMessages.hpp
        include/evtmgr/EventManager.hpp
        include/evtmgr/battery/Thresholds.hpp
)

target_include_directories(evtmgr
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(evtmgr
    PRIVATE
        module-bsp
        module-sys
        service-evtmgr
        sys
)
