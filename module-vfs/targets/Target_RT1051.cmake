set(BOARD_SOURCES ${BOARD_SOURCES}
        vfs_io_syscalls.cpp
        vfs_internal_dirent.cpp
        CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}


        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051 CACHE INTERNAL "")


