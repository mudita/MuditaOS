add_catch2_executable(
    NAME
        utils-conditional-invoke-tests
    SRCS
        test-Utility-ConditionalInvoke.cpp
    LIBS
        module-utils
)

# Phone number tests
add_catch2_executable(
    NAME
        utils-phonenumber
    SRCS
        unittest_phonenumber.cpp
        unittest_numbermatcher.cpp
    LIBS
        module-utils
)

# utils tests
add_catch2_executable(
    NAME
        utils
    SRCS
        unittest_utils.cpp
    LIBS
        module-utils
)

# Log tests
add_catch2_executable(
    NAME
        utils-log
    SRCS
        test_log.cpp
    LIBS
        module-utils
)

# Log dumps tests
add_catch2_executable(
    NAME
        utils-logdumps
    SRCS
        test_logDumps.cpp
    LIBS
        module-utils
    USE_FS
)

# Logger buffer tests
add_catch2_executable(
    NAME
        utils-loggerbuffer
    SRCS
        test_LoggerBuffer.cpp
    LIBS
        module-utils
)
