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

function(add_gtest_executable TESTNAME TESTSRCS)
    add_executable(${TESTNAME} EXCLUDE_FROM_ALL ${TESTSRCS})

    target_link_libraries(${TESTNAME} gtest_main gmock)
    target_compile_options(${TESTNAME} PUBLIC "-pthread")
    target_link_options(${TESTNAME} PUBLIC "-pthread")

    add_dependencies(check ${TESTNAME})
    gtest_add_tests(${TESTNAME} "" AUTO)
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

    # disable logs in unit tests
    if (NOT ${ENABLE_TEST_LOGS})
    	target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-logs.cpp)
    endif (NOT ${ENABLE_TEST_LOGS})

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
