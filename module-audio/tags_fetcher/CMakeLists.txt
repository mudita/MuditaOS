add_library(tagsfetcher STATIC)

module_is_test_entity(tagsfetcher)

target_sources(tagsfetcher
        PRIVATE
        TagsFetcher.cpp
        PUBLIC
        TagsFetcher.hpp)

target_link_libraries(tagsfetcher
    PRIVATE
    tag
    Microsoft.GSL::GSL
)
