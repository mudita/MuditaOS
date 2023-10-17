add_library(module-vfs STATIC)

module_is_test_entity()

add_subdirectory(paths)

target_sources(module-vfs
    PRIVATE
        $<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:
            board/rt1051/newlib/vfs_io_syscalls.cpp
            board/rt1051/purefs/src/fs/thread_local_cwd.cpp
        >
        $<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:
            board/linux/purefs/src/fs/thread_local_cwd.cpp
        >

        drivers/include/purefs/fs/drivers/directory_handle_littlefs.hpp
        drivers/include/purefs/fs/drivers/directory_handle_vfat.hpp
        drivers/include/purefs/fs/drivers/directory_handle_reedgefs.hpp
        drivers/include/purefs/fs/drivers/file_handle_littlefs.hpp
        drivers/include/purefs/fs/drivers/file_handle_vfat.hpp
        drivers/include/purefs/fs/drivers/file_handle_reedgefs.hpp
        drivers/include/purefs/fs/drivers/filesystem_littlefs.hpp
        drivers/include/purefs/fs/drivers/filesystem_ext4.hpp
        drivers/include/purefs/fs/drivers/filesystem_vfat.hpp
        drivers/include/purefs/fs/drivers/filesystem_reedgefs.hpp
        drivers/include/purefs/fs/drivers/mount_point_littlefs.hpp
        drivers/include/purefs/fs/drivers/mount_point_vfat.hpp
        drivers/include/purefs/fs/drivers/mount_point_ext4.hpp
        drivers/include/purefs/fs/drivers/mount_point_reedgefs.hpp
        drivers/include/thirdparty/fatfs/ffconf.h
        drivers/include/thirdparty/reedgefs/redconf.h
        drivers/include/thirdparty/fatfs/volume_mapper.hpp
        drivers/include/thirdparty/littlefs/volume_mapper.hpp
        drivers/include/thirdparty/lwext4/ext4_bdev.hpp
        drivers/src/purefs/fs/filesystem_littlefs.cpp
        drivers/src/purefs/fs/filesystem_ext4.cpp
        drivers/src/purefs/fs/filesystem_vfat.cpp
        drivers/src/purefs/fs/filesystem_reedgefs.cpp
        drivers/src/thirdparty/fatfs/ff_glue.cpp
        drivers/src/thirdparty/fatfs/ffsystem.cpp
        drivers/src/thirdparty/littlefs/lfs_glue.cpp
        drivers/src/thirdparty/lwext4/ext4_bdev.cpp
        drivers/src/purefs/fs/mount_point_ext4.cpp
        drivers/src/thirdparty/reedgefs/redconf.c
        drivers/src/thirdparty/reedgefs/glue.cpp
        drivers/src/thirdparty/reedgefs/services/osassert.c
        drivers/src/thirdparty/reedgefs/services/osbdev_custom.h
        drivers/src/thirdparty/reedgefs/services/osbdev.c
        drivers/src/thirdparty/reedgefs/services/osclock.c
        drivers/src/thirdparty/reedgefs/services/osmutex.c
        drivers/src/thirdparty/reedgefs/services/osoutput.c
        drivers/src/thirdparty/reedgefs/services/ostask.c
        drivers/src/thirdparty/reedgefs/services/ostimestamp.c

        include/internal/purefs/blkdev/disk_handle.hpp
        include/internal/purefs/blkdev/partition_parser.hpp
        include/internal/purefs/fs/notifier.hpp
        include/internal/purefs/fs/thread_local_cwd.hpp
        include/internal/purefs/vfs_subsystem_internal.hpp

        src/purefs/blkdev/disk_handle.cpp
        src/purefs/blkdev/disk_manager.cpp
        src/purefs/blkdev/disk.cpp
        src/purefs/blkdev/partition_parser.cpp
        src/purefs/fs/filesystem_cwd.cpp
        src/purefs/fs/filesystem_operations.cpp
        src/purefs/fs/filesystem_syscalls.cpp
        src/purefs/fs/filesystem.cpp
        src/purefs/fs/fsnotify.cpp
        src/purefs/fs/notifier.cpp
        src/purefs/vfs_subsystem.cpp

    PUBLIC
        include/user/newlib/vfs_io_syscalls.hpp
        include/user/purefs/blkdev/defs.hpp
        include/user/purefs/blkdev/disk_manager.hpp
        include/user/purefs/blkdev/disk.hpp
        include/user/purefs/blkdev/partition.hpp
        include/user/purefs/fs/directory_handle.hpp
        include/user/purefs/fs/file_handle.hpp
        include/user/purefs/fs/filesystem_operations.hpp
        include/user/purefs/fs/filesystem.hpp
        include/user/purefs/fs/fsnotify.hpp
        include/user/purefs/fs/handle_mapper.hpp
        include/user/purefs/fs/inotify_flags.hpp
        include/user/purefs/fs/inotify_message.hpp
        include/user/purefs/fs/inotify.hpp
        include/user/purefs/fs/mount_flags.hpp
        include/user/purefs/fs/mount_point.hpp
        include/user/purefs/vfs_subsystem.hpp
)

target_compile_definitions(module-vfs PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(module-vfs PUBLIC ${PROJECT_TARGET})
target_include_directories(module-vfs PUBLIC ${PROJECT_INCLUDES})
target_include_directories(module-vfs PUBLIC ${CMAKE_BINARY_DIR})

target_include_directories(module-vfs
    PRIVATE
        $<BUILD_INTERFACE:
            $<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/purefs/include>
            $<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:${CMAKE_CURRENT_SOURCE_DIR}/board/linux/purefs/include>
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty/fatfs
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty/reedgefs
            ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
        >
    PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/user/
        >
)

target_link_libraries(module-vfs 
    PRIVATE
        reliance-edge::fs
        fatfs::fatfs
        lwext4::lwext4
        json::json
        littlefs::littlefs
        module-bsp 
        module-os
        module-utils
        pure-core

        PUBLIC
        purefs-paths
        module-sys
        )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
