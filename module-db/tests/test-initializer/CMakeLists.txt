if (NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/sys/user")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/sys/user")
endif()

add_catch2_executable(
    NAME
        db-initializer
    SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests-main.cpp"
        
    LIBS
        module-db
        module-vfs
        iosyscalls
    DEPS
        PurePhone.img-target
)
