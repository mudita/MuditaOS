set(BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/purefs/src/blkdev/disk_image.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/purefs/src/fs/thread_local_cwd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/purefs/src/vfs_subsystem_internal.cpp
        CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/purefs/include/
        CACHE INTERNAL ""
)
