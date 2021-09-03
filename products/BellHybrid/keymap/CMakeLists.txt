add_library(keymap INTERFACE)
add_library(bell::keymap ALIAS keymap)

target_sources(keymap
    PUBLIC
        include/keymap/KeyMap.hpp
)

target_include_directories(keymap
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
