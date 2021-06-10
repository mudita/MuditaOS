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
        iosyscalls
    DEPS
        PurePhone-disk-img
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

# Logger buffer tests
add_catch2_executable(
    NAME
        utils-loggerbuffer
    SRCS
        test_LoggerBuffer.cpp
    LIBS
        module-utils
)

# ParserICS tests
#add_catch2_executable(
#    NAME
#        utils-parserIcs
#    SRCS
#        test_ParserICS.cpp
#    LIBS
#        module-utils
#)
