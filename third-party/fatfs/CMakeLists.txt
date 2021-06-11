add_library(fatfs STATIC)
target_sources(fatfs
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source/ff.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source/ffunicode.c
)
target_include_directories(fatfs
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fatfs/source
)
target_link_libraries(fatfs PRIVATE module-vfs)