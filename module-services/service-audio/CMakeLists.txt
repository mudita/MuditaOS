# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(service-audio STATIC)

target_include_directories(service-audio
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/service-audio>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(service-audio
    PRIVATE
        AudioServiceAPI.cpp
        ServiceAudio.cpp
    PUBLIC
        include/service-audio/AudioMessage.hpp
        include/service-audio/AudioServiceAPI.hpp
        include/service-audio/ServiceAudio.hpp
)

option(ENABLE_PLAYBACK_AUTO_REPEAT "Enable Playback auto repeat" OFF)
if(${ENABLE_PLAYBACK_AUTO_REPEAT})
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_PLAYBACK_AUTO_REPEAT=1)
endif()

target_link_libraries(service-audio
    PRIVATE
        log
        module-bluetooth
        module-utils
        service-bluetooth
        service-evtmgr
    PUBLIC
        apps-common
        messagetype
        module-audio
        module-sys
        service-appmgr
        service-db
)
