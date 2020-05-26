cmake_minimum_required(VERSION 3.12)

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
# UTF8 unit tests project
project(unittest_module-utils_utf8 VERSION 1.0
    DESCRIPTION "Module-utils UTF8 tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_utf8.cpp
    ../utf8/UTF8.cpp
    ../utf8/UTF8.hpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TESTS)
add_test(NAME ${PROJECT_NAME}
     COMMAND ${PROJECT_NAME}
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})

###########################################
#time conversion unit tests project
project(unittest_module-utils_time VERSION 1.0
       DESCRIPTION "Module-utils time unit tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_time_conversion.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils module-vfs)
add_test(NAME ${PROJECT_NAME}
     COMMAND ${PROJECT_NAME}
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})

###########################################
#Utils unit tests project
project(unittest_module-utils_utils VERSION 1.0
       DESCRIPTION "Module-utils utils unit tests")
add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_utils.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_dependencies(check ${PROJECT_NAME})

###########################################
#Duration unit tests project
project(unittest_module-utils_duration VERSION 1.0
       DESCRIPTION "Module-utils duration unit tests")
add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_duration.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils module-vfs)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_dependencies(check ${PROJECT_NAME})

###########################################
#Clipboard unit tests project
project(unittest_module-utils_clipboard VERSION 1.0
       DESCRIPTION "Module-utils Clipboard unit tests")
add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_clipboard.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_dependencies(check ${PROJECT_NAME})

###########################################
#Utils unit tests project
project(unittest_module-utils_phonenumber VERSION 1.0
       DESCRIPTION "Module-utils phonenumber unit tests")
add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_phonenumber.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils module-vfs)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_dependencies(check ${PROJECT_NAME})

