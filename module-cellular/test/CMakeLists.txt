# Phone number tests
add_catch2_executable(
    NAME
        cellular-URC
    SRCS
        unittest_URC.cpp
    LIBS
        module-sys
        module-cellular 
        time-constants
)

add_catch2_executable(
        NAME
        cellular-response
        SRCS
        unittest_response.cpp
        LIBS
        module-sys
        module-cellular
)

add_catch2_executable(
        NAME
        cellular-parse-result
        SRCS
        unittest_parse_result.cpp
        LIBS
        module-sys
        module-cellular
)

add_catch2_executable(
        NAME
        unittest_ATStream
        SRCS
        unittest_ATStream.cpp
        LIBS
        module-sys
        module-cellular
)

add_catch2_executable(
        NAME
        cellular-cmux
        SRCS
        unittest_cmux.cpp
        LIBS
        module-sys
        module-cellular
)

add_catch2_executable(
        NAME
        unittest_ATURCStream
        SRCS
        unittest_ATURCStream.cpp
        LIBS
        module-sys
        module-cellular
)

add_catch2_executable(
        NAME
        unittest_CellularResult
        SRCS
        unittest_CellularResult.cpp
        LIBS
        module-sys
        module-cellular
        module-bsp
)
