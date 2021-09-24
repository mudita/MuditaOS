add_library(time-constants INTERFACE)

target_include_directories(time-constants INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(time-constants
    PUBLIC
        time/time_constants.hpp
)

add_library(utils-time)

target_sources(utils-time
    PRIVATE
        time/time_conversion.cpp
        time/time_date_validation.cpp
        time/time_conversion_factory.cpp
        time/dateCommon.hpp
        time/TimeRangeParser.cpp
        time/TimeZone.cpp
)

target_include_directories(utils-time
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(utils-time
    PRIVATE
        utz::utz

    PUBLIC
        time-constants
        date
        i18n
        log-api
        utility
        utf8
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif()
