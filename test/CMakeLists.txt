option (ENABLE_TEST_LOGS "Enable logs in unit tests" OFF)

set(CATCH2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Catch2")
list(APPEND CMAKE_MODULE_PATH "${CATCH2_PATH}/contrib/")

add_subdirectory(googletest)
add_subdirectory(${CATCH2_PATH})

include(CMakeParseArguments)
include(GoogleTest)
include(Catch)

add_custom_target(unittests)

set(ROOT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

function(add_gtest_executable)
    cmake_parse_arguments(
        _TEST_ARGS
        ""
        "NAME"
        "SRCS;INCLUDE;LIBS;DEFS;DEPS"
        ${ARGN}
    )

    if(NOT _TEST_ARGS_NAME)
        message(FATAL_ERROR "You must provide a test name")
    endif(NOT _TEST_ARGS_NAME)
    set(_TESTNAME "googletest-${_TEST_ARGS_NAME}")

    if(NOT _TEST_ARGS_SRCS)
	message(FATAL_ERROR "You must provide test sources for ${_TESTNAME}")
    endif(NOT _TEST_ARGS_SRCS)

    add_executable(${_TESTNAME} EXCLUDE_FROM_ALL ${_TEST_ARGS_SRCS})

    target_compile_options(${_TESTNAME} PUBLIC "-fsanitize=address")
    target_link_options(${_TESTNAME} PUBLIC "-fsanitize=address")

    # disable logs in unit tests
    if (NOT ${ENABLE_TEST_LOGS})
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-logs.cpp)
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-freertos-tls.cpp)
    endif (NOT ${ENABLE_TEST_LOGS})

    target_link_libraries(${_TESTNAME} PRIVATE gtest_main gmock)
    foreach(lib ${_TEST_ARGS_LIBS})
        target_link_libraries(${_TESTNAME} PRIVATE ${lib})
    endforeach(lib)

    foreach(include ${_TEST_ARGS_INCLUDE})
        target_include_directories(${_TESTNAME} PRIVATE ${include})
    endforeach(include)

    foreach(def ${_TEST_ARGS_DEFS})
        target_compile_definitions(${_TESTNAME} PRIVATE ${def})
    endforeach(def)

    foreach(dep ${_TEST_ARGS_DEPS})
        add_dependencies(${_TESTNAME}  ${dep})
    endforeach(dep)

    target_compile_options(${_TESTNAME} PUBLIC "-pthread")
    target_link_options(${_TESTNAME} PUBLIC "-pthread")

    add_dependencies(unittests ${_TESTNAME})
    add_dependencies(check ${_TESTNAME})
    
    gtest_add_tests(${_TESTNAME} "" AUTO)
endfunction()

function(add_catch2_executable)
    cmake_parse_arguments(
        _TEST_ARGS
        ""
        "NAME"
        "SRCS;INCLUDE;LIBS;DEFS;DEPS"
        ${ARGN}
    )

    if(NOT _TEST_ARGS_NAME)
        message(FATAL_ERROR "You must provide a test name")
    endif(NOT _TEST_ARGS_NAME)
    set(_TESTNAME "catch2-${_TEST_ARGS_NAME}")

    if(NOT _TEST_ARGS_SRCS)
	    message(FATAL_ERROR "You must provide test sources for ${_TESTNAME}")
    endif(NOT _TEST_ARGS_SRCS)

    add_executable(${_TESTNAME} EXCLUDE_FROM_ALL ${_TEST_ARGS_SRCS})

    target_compile_options(${_TESTNAME} PUBLIC "-fsanitize=address")
    target_link_options(${_TESTNAME} PUBLIC "-fsanitize=address")

    # disable logs in unit tests
    if (NOT ${ENABLE_TEST_LOGS} AND NOT ${_TESTNAME} STREQUAL "catch2-utils-log")
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-logs.cpp)
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-freertos-tls.cpp)
    endif (NOT ${ENABLE_TEST_LOGS} AND NOT ${_TESTNAME} STREQUAL "catch2-utils-log")

    target_link_libraries(${_TESTNAME} PRIVATE Catch2::Catch2)
    foreach(lib ${_TEST_ARGS_LIBS})
        target_link_libraries(${_TESTNAME} PRIVATE ${lib})
    endforeach(lib)

    foreach(include ${_TEST_ARGS_INCLUDE})
        target_include_directories(${_TESTNAME} PRIVATE ${include})
    endforeach(include)

    foreach(def ${_TEST_ARGS_DEFS})
        target_compile_definitions(${_TESTNAME} PRIVATE ${def})
    endforeach(def)

    foreach(dep ${_TEST_ARGS_DEPS})
        add_dependencies(${_TESTNAME}  ${dep})
    endforeach(dep)

    add_dependencies(unittests ${_TESTNAME})
    add_dependencies(check ${_TESTNAME})
    catch_discover_tests(${_TESTNAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

set(_CATCH_DISCOVER_TESTS_SCRIPT ${_CATCH_DISCOVER_TESTS_SCRIPT} PARENT_SCOPE)
