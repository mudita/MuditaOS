add_library(time-constants INTERFACE)

target_include_directories(time-constants INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(time-constants 
    PUBLIC
        time/time_constants.hpp
)

add_library(time)

module_is_test_entity(time)

target_sources(time
    PRIVATE
        time/time_conversion.cpp
        time/time_date_validation.cpp
        time/TimeRangeParser.cpp
        time/TimeZone.cpp
        time/time_conversion_factory.cpp

        time/dateCommon.hpp
)

target_include_directories(time PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(time 
    PRIVATE 
        time-constants
        utz::utz
    PUBLIC
        i18n
        log
        module-utils
        utf8
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif()
