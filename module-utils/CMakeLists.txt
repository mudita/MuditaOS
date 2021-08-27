project(module-utils VERSION 1.0)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

module_is_test_entity()

set (SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneNumber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneNumber.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/NumberHolderMatcher.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/country.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ical/ParserICS.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer/StringCircularBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generators/RandomStringGenerator.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    application-calendar
    board
    libphonenumber::libphonenumber
    log
    magic_enum
    module-os
    module-vfs
PRIVATE
    hash-library::hash-library
    json::json
    time
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME}
        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()

add_subdirectory(bootconfig)
add_subdirectory(Clipboard)
add_subdirectory(EventStore)
add_subdirectory(i18n)
add_subdirectory(log)
add_subdirectory(math)
add_subdirectory(rrule)
add_subdirectory(time)
add_subdirectory(ucs2)
add_subdirectory(utf8)
