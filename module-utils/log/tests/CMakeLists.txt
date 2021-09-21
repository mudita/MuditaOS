
# Log tests
add_catch2_executable(
    NAME
        utils-log
    SRCS
        test_log.cpp
    LIBS
        module-utils
        module-vfs
    USE_FS
)

# Log dumps tests
add_catch2_executable(
    NAME
        utils-logdumps
    SRCS
        test_logDumps.cpp
    LIBS
        module-utils
        module-vfs
    USE_FS
)

# Logger buffer tests
add_catch2_executable(
    NAME
        utils-loggerbuffer
    SRCS
        test_LoggerBuffer.cpp
        RandomStringGenerator.cpp
        RandomStringGenerator.hpp
    LIBS
        module-utils
        log
)
