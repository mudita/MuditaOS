# iosyscalls tests
add_catch2_executable(
    NAME
        iosyscalls
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_iosys.cpp
    LIBS
        module-sys
        module-vfs iosyscalls
    DEPS
        PurePhone-disk-img
)
