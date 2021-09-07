add_library(ucs2 STATIC)

module_is_test_entity(ucs2)

target_sources(ucs2
    PRIVATE
       ucs2/UCS2.cpp
)

target_include_directories(ucs2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(ucs2
    PUBLIC
        utf8

    PRIVATE
        log
)

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()
