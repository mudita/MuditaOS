cmake_minimum_required(VERSION 3.12)

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

# UCS2 tests
add_catch2_executable(
    NAME
        utils-ucs2
    SRCS
        unittest_ucs2.cpp
    LIBS
        module-utils
)

# duration tests
add_catch2_executable(
    NAME
        utils-duration
    SRCS
        unittest_duration.cpp
    LIBS
        module-utils
        iosyscalls
    DEPS
        PurePhone-disk-img
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

# Math tests
add_catch2_executable(
    NAME
        utils-math
    SRCS
        test_math.cpp
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

#  i18n language change tests
add_catch2_executable(
    NAME
        utils-i18n
    SRCS
        test_i18n.cpp
    LIBS
        module-utils
)

