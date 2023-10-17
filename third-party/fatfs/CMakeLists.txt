add_library(fatfs INTERFACE)
add_library(fatfs::fatfs ALIAS fatfs)
target_sources(fatfs
    INTERFACE
        src/source/ff.c
        src/source/ffunicode.c
)
target_include_directories(fatfs
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source>
)
