# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
add_library(bell-audio STATIC)
add_library(bell::audio ALIAS bell-audio)

target_include_directories(bell-audio
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/audio>
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_sources(bell-audio
        PRIVATE
        ServiceAudio.cpp

        PUBLIC
        include/audio/AudioMessage.hpp
        include/audio/ServiceAudio.hpp

        )


target_link_libraries(bell-audio
        PRIVATE
        log
        module-utils
        service-db
        PUBLIC
        messagetype
        module-audio
        module-sys
        )
