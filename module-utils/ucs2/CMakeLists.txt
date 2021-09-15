add_library(ucs2 STATIC)

module_is_test_entity(ucs2)

target_sources(ucs2
    PRIVATE
        ucs2/UCS2.cpp

    PUBLIC
        ucs2/UCS2.hpp
)

target_include_directories(ucs2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(ucs2
    PUBLIC
        utf8

    PRIVATE
        log-api
)

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()
