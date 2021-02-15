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
        ${CMAKE_CURRENT_SOURCE_DIR}/i18n/i18n.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/json/json11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utf8/UTF8.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/time_conversion.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common_data/EventStore.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common_data/Clipboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ucs2/UCS2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/time_date_validation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/microtar/src/microtar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/crc32/crc32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneNumber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneNumber.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/NumberHolderMatcher.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/country.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/state/ServiceState.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ical/ParserICS.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/TimeRangeParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/time/DateAndTimeSettings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/LoggerBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer/StringCircularBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generators/RandomStringGenerator.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

include(segger/rtt/CMakeLists.txt)

# add third party libs
include(third-party/re2.cmake)
include(third-party/protobuf-lite.cmake)
include(third-party/libphonenumber.cmake)
include(third-party/tinyexpr.cmake)
include(third-party/pugixml.cmake)

# link against libphonenumber
target_link_libraries (${PROJECT_NAME} PUBLIC ${LIBPHONENUMBER_TARGET})

add_subdirectory(taglib EXCLUDE_FROM_ALL)
set( TAGLIB_INCLUDE_DIRS
	 "${CMAKE_CURRENT_SOURCE_DIR}/taglib/taglib/toolkit"
	 "${CMAKE_CURRENT_SOURCE_DIR}/taglib/taglib"
	 "${CMAKE_CURRENT_BINARY_DIR}/taglib"
	 PARENT_SCOPE
)

# link against tinyexpr
target_link_libraries (${PROJECT_NAME} PUBLIC ${TINYEXPR_TARGET})

# link against pugixml
target_link_libraries (${PROJECT_NAME} PUBLIC ${PUGIXML_TARGET})

target_link_libraries(${PROJECT_NAME} PUBLIC module-os module-vfs)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES} )
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
#target_compile_options( ${PROJECT_NAME} PUBLIC "-Wno-error=deprecated-declarations")
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})
target_include_directories(${PROJECT_NAME}
        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/gsl
        ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include
)

if((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${SYSTEM_VIEW_ENABLED}))
    add_subdirectory(segger/systemview)
endif()

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()

add_subdirectory(bootconfig)
