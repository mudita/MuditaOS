add_catch2_executable(
    NAME
        utils-conditional-invoke-tests
    SRCS
        test-Utility-ConditionalInvoke.cpp
    LIBS
        module-utils
)

add_catch2_executable(
    NAME
        utils
    SRCS
        unittest_utils.cpp
    LIBS
        module-utils
)

