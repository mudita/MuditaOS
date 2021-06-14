add_library(fatfs STATIC)
add_library(fatfs::fatfs ALIAS fatfs)
target_sources(fatfs
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source/ff.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source/ffunicode.c
)
target_include_directories(fatfs
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source>
)
target_link_libraries(fatfs PUBLIC module-vfs)