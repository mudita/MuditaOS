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
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/BluetoothWorker.cpp
)

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
endif()

if(${BT_STACK} STREQUAL "Bluetopia")
    if(NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
        message("Build with bluetopia")
        include(lib/Bluetopia.cmake)
    else()
        message("Nor bluetopia not bluekitchen - wont compile")
    endif()
else()
    message("Build with BlueKitchen")
    include(lib/btstack.cmake)
endif()


message("Sources: " ${SOURCES})
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_DIR_SOURCES})


# # Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${TARGET_COMPILE_DEFINITIONS}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TARGET_LIBRARIES_INCLUDES}
)

target_link_libraries(
    ${PROJECT_NAME}
    module-bsp
    module-utils
    module-vfs
    module-sys
    ${BOARD_DIR_LIBRARIES}
    )

# Host target configuration(mainly used for unit testing)
if(DEFINED BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()

