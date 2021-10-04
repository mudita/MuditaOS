add_library(evtmgr STATIC)
add_library(bell::evtmgr ALIAS evtmgr)

target_sources(evtmgr
    PRIVATE
        EventManager.cpp
        WorkerEvent.cpp
        WorkerEvent.hpp
        internal/StaticData.cpp
        internal/TemperatureApi.cpp
        internal/key_sequences/KeySequenceMgr.cpp
        internal/StaticData.hpp
        screen-light-control/ScreenLightControl.cpp
        backlight-handler/BacklightHandler.cpp

        internal/key_sequences/AbstractKeySequence.hpp
        internal/key_sequences/KeySequenceMgr.hpp
        internal/key_sequences/PowerOffSequence.hpp
        internal/key_sequences/AlarmActivateSequence.hpp
        internal/key_sequences/AlarmDeactivateSequence.hpp
        internal/key_sequences/ReleaseSequence.hpp
        internal/key_sequences/ResetSequence.hpp
        internal/key_sequences/GenericLongPressSequence.hpp
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
        bell::appmgr
        sys
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
