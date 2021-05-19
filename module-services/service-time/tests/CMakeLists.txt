cmake_minimum_required(VERSION 3.12)

add_catch2_executable(
        NAME
        timezone-handler
        SRCS
        unittest_timezone_handler.cpp
        LIBS
        service-time
        module-sys
)
