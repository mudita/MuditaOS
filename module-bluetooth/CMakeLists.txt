set(CMAKE_CXX_STANDARD 14)

project(module-bluetooth VERSION 1.0
        DESCRIPTION "Cellular module library")

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
    ${CMAKE_CURRENT_SOURCE_DIR}/BT/BluetoothWorker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BT/Stack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BT/profiles/A2DP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BT/profiles/GAVD.cpp
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/hcitrans/HCITRANS.c
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/src/A3DPDemo_SRC.c
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/src/HAL.c
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/src/bt_port.c
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/src/bt_dev.c
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/src/bt_DEVICE_CLASS_tostr.c
    #    ${CMAKE_CURRENT_LIST_DIR}/lib/src/bt_util.c
    )

if(NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
else()
    include(targets/Target_Linux.cmake)
endif()

message("Sources: " ${SOURCES})

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

# # Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${TARGET_COMPILE_DEFINITIONS}
                                                    -D__SUPPORT_CC256XC_PATCH__
                                                    -DDEBUG_ENABLED
                                                    )
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

if(NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/btpskrnl
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/btpsvend
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/btvs
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/hcitrans/
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/AUDIO/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/A2DP/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/AVCTP/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/AVRCP/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/GAVD/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/src
)

find_library(Bluetopia_32_M7 Bluetopia_32_M7.fp.dp PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/lib/gcc/)
find_library(SS1BTAUD_32_M7  SS1BTAUD_32_M7.fp.dp PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/AUDIO/lib/gcc)
find_library(SS1BTAVC_32_M7  SS1BTAVC_32_M7.fp.dp PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/AVCTP/lib/gcc)
find_library(SS1BTAVR_32_M7  SS1BTAVR_32_M7.fp.dp PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/AVRCP/lib/gcc)
find_library(SS1BTGAV_32_M7  SS1BTGAV_32_M7.fp.dp PATHS ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetopia/profiles/GAVD/lib/gcc)

message(STATUS "Bluetopia_32_M7: " ${Bluetopia_32_M7})
message(STATUS "SS1BTAUD_32_M7:  " ${SS1BTAUD_32_M7})
message(STATUS "SS1BTAVC_32_M7:  " ${SS1BTAVC_32_M7})
message(STATUS "SS1BTAVR_32_M7:  " ${SS1BTAVR_32_M7})
message(STATUS "SS1BTGAV_32_M7:  " ${SS1BTGAV_32_M7})

target_link_libraries(
    ${PROJECT_NAME}
    ${Bluetopia_32_M7}
    ${SS1BTAUD_32_M7}
    ${SS1BTAVC_32_M7}
    ${SS1BTAVR_32_M7}
    ${SS1BTGAV_32_M7}
    module-bsp
    module-utils
    module-vfs
    module-sys
)
else()
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Host target configuration(mainly used for unit testing)
if(DEFINED BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()

