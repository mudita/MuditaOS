cmake_minimum_required(VERSION 3.12)

project(module-audio VERSION 1.0
        DESCRIPTION "Audio module library")

if(NOT DEFINED PROJECT_LIB_DIRECTORY )
    set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})

if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
    set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
    message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

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
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Operation/IdleOperation.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Profiles/Profile.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/State/State.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/State/StateMachine.cpp"

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
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(${PROJECT_NAME} module-bsp module-utils module-vfs  module-sys )

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

    add_subdirectory(tests)

endif()

