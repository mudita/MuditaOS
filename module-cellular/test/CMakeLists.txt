cmake_minimum_required(VERSION 3.12)

# Phone number tests
add_catch2_executable(
    NAME
        cellular-URC
    SRCS
        unittest_URC.cpp
    LIBS
        module-cellular 
)

add_catch2_executable(
        NAME
        cellular-response
        SRCS
        unittest_response.cpp
        LIBS
        module-cellular
)

add_catch2_executable(
        NAME
        cellular-parse-result
        SRCS
        unittest_parse_result.cpp
        LIBS
        module-cellular
)

