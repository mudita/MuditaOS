cmake_minimum_required(VERSION 3.12)

# iosyscalls tests
add_catch2_executable(
    NAME
        iosyscalls
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_iosys.cpp
    LIBS
        module-vfs iosyscalls
    DEPS
        disk_image
)
