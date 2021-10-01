add_library(klib INTERFACE)
add_library(klib::klib ALIAS klib)

target_include_directories(klib
    INTERFACE
        $<BUILD_INTERFACE:
             ${CMAKE_CURRENT_SOURCE_DIR}/src
             ${CMAKE_CURRENT_SOURCE_DIR}/include
        >
)
