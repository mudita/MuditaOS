cmake_minimum_required(VERSION 3.12)

add_catch2_executable(
        NAME
            cellular-mmi
        SRCS
            unittest_mmi.cpp
        LIBS
            module-cellular
)

add_catch2_executable(
        NAME
        cellular-simcard
        SRCS
        unittest_simcard.cpp
        LIBS
        module-cellular
)


add_catch2_executable(
        NAME
        cellular-datatransfer
        SRCS
        unittest_datatransfer.cpp
        LIBS
        module-cellular
)