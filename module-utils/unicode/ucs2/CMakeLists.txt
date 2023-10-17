add_library(ucs2 STATIC)

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
        log-api
)

if (${ENABLE_TESTS})
    add_subdirectory( test )
endif()
