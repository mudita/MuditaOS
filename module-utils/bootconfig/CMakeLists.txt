add_library(utils-bootconfig STATIC)

target_sources(utils-bootconfig
    PRIVATE
        src/bootconfig.cpp
)

target_include_directories(utils-bootconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>    
)

target_link_libraries(utils-bootconfig
   PRIVATE
        module-os
        purefs-paths
        utils-time
        utility
        version-header

        Microsoft.GSL::GSL
        json::json
        hash-library::hash-library
)
