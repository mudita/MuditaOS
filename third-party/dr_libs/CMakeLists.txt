
add_library(dr_libs INTERFACE)

target_include_directories(dr_libs
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
        )

target_sources(dr_libs
        INTERFACE
        src/dr_flac.h
        src/dr_mp3.h
        src/dr_wav.h
        )
