add_library(tagsfetcher STATIC)

module_is_test_entity(tagsfetcher)

target_sources(tagsfetcher
        PRIVATE
        TagsFetcher.cpp
        PUBLIC
        TagsFetcher.hpp)

target_include_directories(ucs2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(tagsfetcher
    PRIVATE
    tag
    Microsoft.GSL::GSL
    module-utils
)
