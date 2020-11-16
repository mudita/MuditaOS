set(BOARD_SOURCES ${BOARD_SOURCES}
        src/newlib/vfs_io_syscalls.cpp
        src/newlib/vfs_internal_dirent.cpp
        src/newlib/HandleManager.cpp
        CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}


        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051 CACHE INTERNAL "")


