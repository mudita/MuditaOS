set(BOARD_SOURCES ${BOARD_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/purefs/src/blkdev/disk_emmc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/purefs/src/blkdev/disk_eeprom.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/purefs/src/fs/thread_local_cwd.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/purefs/src/vfs_subsystem_internal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/newlib/vfs_io_syscalls.cpp
        CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051
    ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/purefs/include/
    CACHE INTERNAL ""
)


