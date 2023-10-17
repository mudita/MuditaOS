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
