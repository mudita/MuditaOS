set(BOARD_SOURCES ${BOARD_SOURCES}
        src/newlib/vfs_io_syscalls.cpp
        src/newlib/vfs_internal_dirent.cpp
        src/newlib/HandleManager.cpp
        board/rt1051/purefs/src/fs/thread_local_cwd.cpp
        board/rt1051/purefs/src/vfs_subsystem_internal.cpp
        CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051
    CACHE INTERNAL ""
)


