cmake_minimum_required(VERSION 3.12)

project(module-audio VERSION 1.0
        DESCRIPTION "Audio module library")

set(SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoder.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderFLAC.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderMP3.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/decoderWAV.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/decoder/xing_header.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/encoder/Encoder.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/encoder/EncoderWAV.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Audio.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/Operation.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/PlaybackOperation.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/RecorderOperation.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/RouterOperation.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/IdleOperation.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Profiles/Profile.cpp"
        )

if(NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
else()
    include(targets/Target_Linux.cmake)
endif()


add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${SERIAL_PORT_PATH})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME}
    PUBLIC
    ${TARGET_COMPILE_OPTIONS}
    PRIVATE
    -Wno-error
)
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(${PROJECT_NAME} module-bsp module-os module-utils module-vfs  module-sys )

# Host target configuration(mainly used for unit testing)

message(WARNING "Tests for ${PROJECT_NAME} need update - disabled: https://appnroll.atlassian.net/browse/EGD-2983")
#add_subdirectory(tests)
