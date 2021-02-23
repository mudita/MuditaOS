cmake_minimum_required(VERSION 3.12)

project(module-cellular VERSION 1.0
        DESCRIPTION "Cellular module library")

include(SerialPort)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/ATParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/ATStream.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Modem/ATURCStream.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/Urc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcQind.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCusd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCtze.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCreg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCmti.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcClip.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcCpin.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcQiurc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcPoweredDown.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcRing.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcResponse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/UrcFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/Commands.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/src/Cmd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/response.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CSCA.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/QECCNUM.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/at/cmd/src/CLCC.cpp
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
define_serial(${PROJECT_NAME})
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
        ${CMAKE_CURRENT_SOURCE_DIR}/at/
        ${CMAKE_SOURCE_DIR}/module-services

)

target_link_libraries(${PROJECT_NAME}
    service-cellular
    service-eink
    service-fota
    module-utils
    module-vfs
    module-services
    module-sys
    module-db
    module-bsp
)

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()
