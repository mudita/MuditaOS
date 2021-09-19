# Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set(AUDIO_RT1051_SOURCES
    "SAIAudioDevice.cpp"
    "RT1051AudioCodec.cpp"
    "RT1051CellularAudio.cpp"
    "RT1051DeviceFactory.cpp"
    "RT1051Platform.cpp"
)

add_library(${AUDIO_BOARD_LIBRARY} STATIC ${AUDIO_RT1051_SOURCES})
target_include_directories(${AUDIO_BOARD_LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${AUDIO_BOARD_LIBRARY}
    module-bsp
    module-os
    module-bluetooth
)
