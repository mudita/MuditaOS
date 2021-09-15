add_library(bootconfig STATIC)
target_sources(bootconfig PRIVATE src/bootconfig.cpp)

target_include_directories(bootconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(bootconfig
   PRIVATE
      purefs-paths
      hash-library::hash-library
      Microsoft.GSL::GSL
      json::json
      version-header
)
