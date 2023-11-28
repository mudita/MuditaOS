add_library(lzma STATIC)

target_include_directories(lzma
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_sources(lzma
        PUBLIC
        lzma.cpp
)

target_link_libraries(lzma PUBLIC minlzma)

#if (${ENABLE_TESTS})
#    add_subdirectory(tests)
#endif() #TODO
