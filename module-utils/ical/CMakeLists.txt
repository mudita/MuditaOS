add_library(utils-calendar STATIC)

target_sources(utils-calendar
    PUBLIC
        ParserICS.hpp       

    PRIVATE
        ParserICS.cpp
)

target_include_directories(utils-calendar
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(utils-calendar
    PUBLIC
        utils-time
        json::json
)

if (${ENABLE_TESTS})
    # calendar tests are out of date
    # add_subdirectory(tests)
endif()
