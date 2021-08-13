function(add_catch2_executable)
    cmake_parse_arguments(
            _TEST_ARGS
            "USE_FS"
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

    get_directory_property(_TEST_ENTITY TEST_ENTITY)

    add_executable(${_TESTNAME} EXCLUDE_FROM_ALL ${_TEST_ARGS_SRCS})

    target_compile_options(${_TESTNAME} PUBLIC "-fsanitize=address")
    target_link_options(${_TESTNAME} PUBLIC "-fsanitize=address")

    # disable logs in unit tests
    if (NOT ${ENABLE_TEST_LOGS} AND NOT ${_TESTNAME} STREQUAL "catch2-utils-log")
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-logs.cpp)
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-freertos-tls.cpp)
    endif (NOT ${ENABLE_TEST_LOGS} AND NOT ${_TESTNAME} STREQUAL "catch2-utils-log")

    set(_TEST_LABELS "")
    if(_TEST_ARGS_USE_FS)
        enable_test_filesystem()
    endif()

    target_link_libraries(${_TESTNAME} PRIVATE Catch2::Catch2 log)
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

    if(_TEST_ENTITY)
        add_dependencies(unittests-${_TEST_ENTITY} ${_TESTNAME})
        list(APPEND _TEST_LABELS ${_TEST_ENTITY})
    endif()

    catch_discover_tests(${_TESTNAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            PROPERTIES LABELS ${_TEST_LABELS}
            )
endfunction()
