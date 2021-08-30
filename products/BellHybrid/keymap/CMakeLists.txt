add_library(keymap INTERFACE)

target_sources(keymap
    PUBLIC
        include/keymap/KeyMap.hpp
)

target_include_directories(keymap
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
