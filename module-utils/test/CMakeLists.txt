cmake_minimum_required(VERSION 3.12)

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

# Clipboard tests
add_catch2_executable(
    NAME
        utils-clipboard
    SRCS
        unittest_clipboard.cpp
    LIBS
        module-utils
)

# UCS2 tests
add_catch2_executable(
    NAME
        utils-ucs2
    SRCS
        unittest_ucs2.cpp
        ../ucs2/UCS2.cpp
        ../ucs2/UCS2.hpp
    LIBS
        module-utils
)

# UTF8 tests
add_catch2_executable(
    NAME
        utils-utf8
    SRCS
        unittest_utf8.cpp
        ../utf8/UTF8.cpp
        ../utf8/UTF8.hpp
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
        PurePhone.img-target
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
        PurePhone.img-target
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

#  time display tests
#add_catch2_executable(
#    NAME
#        utils-time_display
#    SRCS
#        unittest_TimeRangeParser.cpp
#    LIBS
#        module-utils
#        module-db
#)

###########################################
# Log functional tests project
project(test_module-utils_log VERSION 1.0
    DESCRIPTION "Module-utils log functional tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    test_log.cpp
    test_log2.cpp
)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)

###########################################
# UTF8 functional tests projeet
project(test_module-utils_utf8 VERSION 1.0
    DESCRIPTION "Module-utils utf8 functional tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    test_utf8.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})

###########################################
# time conversion tests project
project(test_module-utils_time VERSION 1.0
    DESCRIPTION "Module-utils time tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    test_time_conversion.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils module-vfs)
add_test(NAME ${PROJECT_NAME}
     COMMAND ${PROJECT_NAME}
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})
