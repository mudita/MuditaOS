add_library(minimp3 INTERFACE)
add_library(minimp3::minimp3 ALIAS minimp3)

target_include_directories(minimp3
    INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/minimp3>
)
