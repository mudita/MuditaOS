cmake_minimum_required(VERSION 3.12)

if(NOT WIN32)
    include(Colours)
endif()

project(module-utils VERSION 1.0)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

module_is_test_entity()

add_subdirectory(Clipboard)
add_subdirectory(EventStore)

set (SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/i18n/i18n.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/json/json11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/time_conversion.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ucs2/UCS2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/time_date_validation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneNumber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneNumber.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/NumberHolderMatcher.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/country.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ical/ParserICS.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/TimeRangeParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/DateAndTimeSettings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer/StringCircularBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generators/RandomStringGenerator.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

include(segger/rtt/CMakeLists.txt)

# add third party libs
include(third-party/libphonenumber.cmake)

target_link_libraries(${PROJECT_NAME}
PUBLIC 
    board
    log
    magic_enum
    module-os 
    module-vfs
    ${LIBPHONENUMBER_TARGET}
PRIVATE
    crc32
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
#target_compile_options( ${PROJECT_NAME} PUBLIC "-Wno-error=deprecated-declarations")
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})
target_include_directories(${PROJECT_NAME}
        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
)

if((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${SYSTEM_VIEW_ENABLED}))
    add_subdirectory(segger/systemview)
endif()

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()

add_subdirectory(board)
add_subdirectory(bootconfig)
add_subdirectory(log)
add_subdirectory(utf8)
