add_library(utf8 STATIC)

target_sources(utf8
    PRIVATE
        utf8/UTF8.cpp
)

target_include_directories(utf8
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(utf8 PRIVATE log-api)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif()
