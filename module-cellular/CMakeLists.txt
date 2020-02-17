cmake_minimum_required(VERSION 3.12)

project(module-cellular VERSION 1.0
        DESCRIPTION "Cellular module library")

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/ATParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/ATCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/DLC_channel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_CLOSE.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_CONTROL.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_DATA.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_DLC_ESTABL.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_DLC_RELEASE.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_FLOW.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_PARNEG.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_PORTNEG.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_SERVNEG.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_SLEEP.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_START.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_TEST.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/TS0710/TS0710_WAKEUP.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/at/src/URC_QIND.cpp
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
        ${CMAKE_SOURCE_DIR}/module-services

)

target_link_libraries(${PROJECT_NAME} module-bsp module-utils module-vfs  module-sys module-db )

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

    add_subdirectory(tests)

endif()

