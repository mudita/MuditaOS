# Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set(AUDIO_LINUX_SOURCES
	LinuxAudioPlatform.cpp
	LinuxAudioDevice.cpp
)

add_library(${AUDIO_BOARD_LIBRARY} STATIC ${AUDIO_LINUX_SOURCES})
target_include_directories(${AUDIO_BOARD_LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${AUDIO_BOARD_LIBRARY}
    module-os
)
