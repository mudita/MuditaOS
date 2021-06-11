cmake_minimum_required(VERSION 3.13)

add_library(littlefs STATIC)
add_library(littlefs::littlefs ALIAS littlefs)

target_sources(littlefs
    PRIVATE
        src/lfs.c
        src/lfs_util.c
        src/bd/lfs_filebd.c
        src/bd/lfs_rambd.c
        src/bd/lfs_testbd.c
    PUBLIC
        src/lfs.h
        src/lfs_util.h
        src/bd/lfs_filebd.h
        src/bd/lfs_rambd.h
        src/bd/lfs_testbd.h
)

target_compile_definitions(littlefs 
    PUBLIC 
        LFS_THREADSAFE 
)

target_include_directories(littlefs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
