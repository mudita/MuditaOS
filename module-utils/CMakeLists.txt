cmake_minimum_required(VERSION 3.12)

if(NOT WIN32)
    include(Colours)
endif()

project(module-utils VERSION 1.0
       DESCRIPTION "Library for processing utf8 strings.")

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
	include(targets/Target_Linux.cmake)
else()
	include(targets/Target_Cross.cmake)
endif()

set (SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/i18/i18.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/json/json11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utf8/UTF8.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/time_conversion.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common_data/EventStore.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ucs2/UCS2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/time_date_validation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/microtar/src/microtar.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crc32/crc32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sbini/sbini.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

include(segger/rtt/CMakeLists.txt)

# add third party libs
include(third-party/re2.cmake)
include(third-party/protobuf-lite.cmake)
include(third-party/libphonenumber.cmake)

target_link_libraries(${PROJECT_NAME} PUBLIC module-os module-vfs)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES} )
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_include_directories(${PROJECT_NAME}
        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/gsl
)

if((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${SYSTEM_VIEW_ENABLED}))
    add_subdirectory(segger/systemview)
endif()
add_subdirectory( test )
