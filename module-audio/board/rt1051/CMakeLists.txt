# Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(${AUDIO_BOARD_LIBRARY} STATIC)

target_sources(${AUDIO_BOARD_LIBRARY}
    PRIVATE
        RT1051Platform.cpp
        SAIAudioDevice.cpp

    PUBLIC
        SAIAudioDevice.hpp
)

target_compile_definitions(${AUDIO_BOARD_LIBRARY}
    PRIVATE
        BOARD_${BOARD}=1
)

target_include_directories(${AUDIO_BOARD_LIBRARY}
    PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}
        >
)

target_link_libraries(${AUDIO_BOARD_LIBRARY}
    PRIVATE
        module-bsp
        module-os
        module-bluetooth
)

add_subdirectory(${BOARD})
