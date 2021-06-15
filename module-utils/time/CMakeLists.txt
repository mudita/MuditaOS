add_library(time)

module_is_test_entity(time)

target_sources(time
    PRIVATE
        time/DateAndTimeSettings.cpp
        time/time_conversion.cpp
        time/time_date_validation.cpp
        time/TimeRangeParser.cpp
        time/TimeZone.cpp
)

target_include_directories(time PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(time 
    PRIVATE 
        log
        module-utils
        utf8
        utz::utz
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif()
