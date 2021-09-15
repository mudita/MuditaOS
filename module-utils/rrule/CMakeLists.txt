add_library(rrule)

target_sources(rrule
    PRIVATE
        rrule/rrule.cpp

    PUBLIC
        rrule/rrule.hpp
)

target_include_directories(rrule PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(rrule
    PUBLIC
        date
        time
        utils
    PRIVATE
        ical_cxx
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif()
