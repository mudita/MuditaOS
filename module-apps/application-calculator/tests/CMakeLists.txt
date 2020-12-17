if (NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/sys/user")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/sys/user")
endif()

add_catch2_executable(
    NAME
        calculator
    SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/CalculatorUtility_tests.cpp"
    LIBS
        module-apps
        module-utils
        module-vfs
        iosyscalls
    DEPS
        PurePhone.img-target
)
