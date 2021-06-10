cmake_minimum_required(VERSION 3.12)

add_library(module-vfs STATIC)

module_is_test_entity()

set(FF_FAT_SOURCES_THIRDPARTY
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fatfs/source/ff.c
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fatfs/source/ffunicode.c
)

set(FF_FAT_SOURCES
    ${FF_FAT_SOURCES_THIRDPARTY}
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/src/thirdparty/fatfs/ffsystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/src/thirdparty/fatfs/ff_glue.cpp
)
set(FF_LFS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/src/thirdparty/littlefs/lfs_glue.cpp
)

set(SOURCES
    ${FF_FAT_SOURCES}
    ${FF_LFS_SOURCES}
    src/purefs/filesystem_paths.cpp
    src/purefs/blkdev/disk_manager.cpp
    src/purefs/blkdev/disk.cpp
    src/purefs/blkdev/partition_parser.cpp
    src/purefs/blkdev/disk_handle.cpp
    src/purefs/fs/filesystem.cpp
    src/purefs/fs/filesystem_operations.cpp
    src/purefs/fs/filesystem_syscalls.cpp
    src/purefs/fs/filesystem_cwd.cpp
    src/purefs/vfs_subsystem.cpp
    src/purefs/fs/notifier.cpp
    src/purefs/fs/fsnotify.cpp
    drivers/src/purefs/fs/filesystem_vfat.cpp
    drivers/src/purefs/fs/filesystem_littlefs.cpp
)


if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

target_sources(module-vfs PRIVATE ${SOURCES} ${BOARD_SOURCES})

#this is needed as linux iosyscalls produces SHARED code instead of STATIC. Without -fPIC it won't link
if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(TARGET_COMPILE_OPTIONS
        "-fPIC"
        CACHE INTERNAL ""
        )
endif()

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(module-vfs PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(module-vfs PUBLIC ${PROJECT_TARGET})
target_compile_definitions(module-vfs PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(module-vfs PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(module-vfs PUBLIC ${PROJECT_INCLUDES})
target_include_directories(module-vfs PUBLIC ${CMAKE_BINARY_DIR})
target_compile_features(module-vfs PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(module-vfs PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(module-vfs PUBLIC ${TARGET_LINK_OPTIONS})

target_include_directories(module-vfs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/user/
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty/fatfs
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fatfs/source
)

target_link_libraries(module-vfs
    PUBLIC 
        ${TARGET_LIBRARIES} 
        module-os
        module-sys
        module-bsp 
        module-utils
    PRIVATE 
        littlefs::littlefs
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
