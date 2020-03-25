cmake_minimum_required(VERSION 3.12)

# Locate GTest
find_package(GTest)


set( CMAKE_PROJECT_UNIT_TEST_UTF8 "unittest_utf8" )
set( CMAKE_PROJECT_UNIT_TEST_LOG "unittest_log" )
set( CMAKE_PROJECT_GTEST_UTF8 "gtest_utf8" )
#Log unit tests projeet
project(unittest_module-utils_log VERSION 1.0
       DESCRIPTION "Module-utils log unit tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_log.cpp
    unittest_log2.cpp
)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)

###########################################
#UTF8 unit tests projeet
project(unittest_module-utils_utf8 VERSION 1.0
       DESCRIPTION "Module-utils utf8 unit tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
    unittest_utf8.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)
add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_dependencies(check ${PROJECT_NAME})

###########################################
# UTF8 gtest executable
if( ${GTEST_FOUND} )
    add_executable( ${CMAKE_PROJECT_GTEST_UTF8} EXCLUDE_FROM_ALL
        gtest_utf8.cpp)
    target_include_directories(${CMAKE_PROJECT_GTEST_UTF8}
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/.."
        "${GTEST_INCLUDE_DIRS}"
        )
    target_link_libraries( ${CMAKE_PROJECT_GTEST_UTF8}
        #${LIBRARY_UTF8}
        GTest::GTest
        GTest::Main
        pthread
        module-utils
        )
    add_test(NAME ${CMAKE_PROJECT_GTEST_UTF8}
        COMMAND ${CMAKE_PROJECT_GTEST_UTF8}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    add_dependencies(check ${CMAKE_PROJECT_GTEST_UTF8})
else()
    message( "${Red} No GTest ${ColourReset} - skipping build of ${CMAKE_PROJECT_GTEST_UTF8} ")
    message(WARNING "Tests for ${PROJECT_NAME} need update - disabled:  https://appnroll.atlassian.net/browse/EGD-2996")
endif()

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
#Utils unit tests project
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
