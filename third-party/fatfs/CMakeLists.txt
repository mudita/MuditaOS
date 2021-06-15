add_library(fatfs INTERFACE)
add_library(fatfs::fatfs ALIAS fatfs)
target_sources(fatfs
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source/ff.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source/ffunicode.c
)
target_include_directories(fatfs
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source>
)
