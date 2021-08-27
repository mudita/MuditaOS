# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

project(module-audio VERSION 1.0
        DESCRIPTION "Audio module library")

module_is_test_entity()

# include target specific rules
set(AUDIO_BOARD_LIBRARY audio-${PROJECT_TARGET_NAME})
add_subdirectory(board/${PROJECT_TARGET_NAME})
target_include_directories(${AUDIO_BOARD_LIBRARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} STATIC)
target_sources(
                ${PROJECT_NAME}
        PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Audio.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioCommon.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioDeviceFactory.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioFormat.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioMux.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/Decoder.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderFLAC.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderMP3.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderWAV.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/DecoderWorker.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/xing_header.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/encoder/Encoder.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/encoder/EncoderWAV.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Endpoint.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/IdleOperation.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/Operation.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/PlaybackOperation.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/RecorderOperation.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/RouterOperation.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Profiles/Profile.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/ServiceObserver.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/Stream.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/StreamFactory.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/StreamProxy.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/StreamQueuedEventsListener.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/transcode/InputTranscodeProxy.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/transcode/MonoToStereo.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/transcode/NullTransform.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/transcode/TransformComposite.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/transcode/TransformFactory.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/VolumeScaler.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Audio/equalizer/Equalizer.cpp
)

target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# supress warning for flac decoder
set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderFLAC.cpp
        PROPERTIES COMPILE_FLAGS
	"-Wno-implicit-fallthrough -Wno-error=maybe-uninitialized"
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
                ${AUDIO_BOARD_LIBRARY}
                module-bsp
                module-os
                module-utils
                module-sys
                tag
                minimp3::minimp3
        PRIVATE
                math
                tagsfetcher
)

add_subdirectory(tags_fetcher)

if (${ENABLE_TESTS})
    add_subdirectory(Audio/test)
endif ()
