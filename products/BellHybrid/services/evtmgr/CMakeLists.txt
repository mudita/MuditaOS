add_library(evtmgr STATIC)
add_library(bell::evtmgr ALIAS evtmgr)

target_sources(evtmgr
    PRIVATE
        EventManager.cpp
        WorkerEvent.cpp
        WorkerEvent.hpp
        internal/StaticData.cpp
        internal/TemperatureApi.cpp

        internal/StaticData.hpp
        screen-light-control/ScreenLightControl.cpp
    PUBLIC
        include/evtmgr/EventManager.hpp
        include/evtmgr/api/TemperatureApi.hpp
)

target_include_directories(evtmgr
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(evtmgr
   PRIVATE
        keymap
        module-bsp
        module-utils
        service-evtmgr
        sys
)
