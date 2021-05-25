add_library(sml INTERFACE)
add_library(sml::sml ALIAS sml)

target_include_directories(sml
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)
