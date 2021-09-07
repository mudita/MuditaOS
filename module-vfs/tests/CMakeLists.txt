add_catch2_executable(
    NAME vfs-inotify
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_filesystem_inotify.cpp
    LIBS
        module-sys
        module-vfs
    INCLUDE
        $<TARGET_PROPERTY:module-vfs,INCLUDE_DIRECTORIES>
)
