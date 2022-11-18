add_library(purefs-paths STATIC)

target_sources(purefs-paths
    PRIVATE
        filesystem_paths.cpp
        include/purefs/filesystem_paths.hpp
)
target_link_libraries(purefs-paths
        PRIVATE
        pure-core
        )
target_include_directories(purefs-paths
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)
