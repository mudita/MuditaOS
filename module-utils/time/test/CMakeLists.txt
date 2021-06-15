add_catch2_executable(
    NAME
        utils-time_display
    SRCS
        unittest_TimeRangeParser.cpp
    LIBS
        module-utils
        time
)

add_catch2_executable(
    NAME
        utils-TimeZone
    SRCS
        unittest_TimeZone.cpp
    LIBS
        time
)