# Clipboard tests
add_catch2_executable(
    NAME
        utils-clipboard
    SRCS
        unittest_clipboard.cpp
    LIBS
        log-api
        clipboard

)
