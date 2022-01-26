option (ENABLE_TEST_LOGS "Enable logs in unit tests" OFF)

set(CATCH2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Catch2")
list(APPEND CMAKE_MODULE_PATH "${CATCH2_PATH}/contrib/")

add_subdirectory(googletest)
add_subdirectory(${CATCH2_PATH})

define_property(
    DIRECTORY PROPERTY TEST_ENTITY INHERITED
    BRIEF_DOCS "Test entity of the directory"
    FULL_DOCS "Add directory to a test entity to group tests defined in the directory and its subdirectories"
)

include(Assets)
include(Catch)
include(CMakeParseArguments)
include(DiskImage)
include(GoogleTest)

add_custom_target(unittests)

set(ROOT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(TEST_ASSETS_DEST_DIR ${CMAKE_BINARY_DIR}/test-sysroot/sys)
set(TEST_ASSETS_DEST_DIR ${TEST_ASSETS_DEST_DIR} PARENT_SCOPE)

add_assets_target(
    TARGET test-assets
    SOURCE_DIR ${ASSETS_SOURCE_DIR}
    DEST_DIR ${TEST_ASSETS_DEST_DIR}
    DEVEL ON
)

add_image(
    PRODUCT Test
    SYSROOT test-sysroot
    ASSETS  test-assets
    IMAGE_PARTITIONS ${CMAKE_SOURCE_DIR}/config/products/PurePhone/image_partitions.map
    DEPENDS test-assets
)

macro(enable_test_filesystem)
    target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/filesystem.cpp)
    list(APPEND _TEST_ARGS_LIBS iosyscalls)
    list(APPEND _TEST_ARGS_LIBS board)
    list(APPEND _TEST_ARGS_INCLUDE ${CMAKE_SOURCE_DIR}/module-vfs/include/user)
    list(APPEND _TEST_ARGS_DEPS Test-disk-img)
endmacro()

function(add_gtest_executable)
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
    set(_TESTNAME "googletest-${_TEST_ARGS_NAME}")

    if(NOT _TEST_ARGS_SRCS)
	message(FATAL_ERROR "You must provide test sources for ${_TESTNAME}")
    endif(NOT _TEST_ARGS_SRCS)

    get_directory_property(_TEST_ENTITY TEST_ENTITY)

    add_executable(${_TESTNAME} EXCLUDE_FROM_ALL ${_TEST_ARGS_SRCS})

    target_compile_options(${_TESTNAME} PUBLIC "-fsanitize=address")
    target_link_options(${_TESTNAME} PUBLIC "-fsanitize=address")

    # disable logs in unit tests
    if (NOT ${ENABLE_TEST_LOGS})
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-logs.cpp)
        target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-freertos-tls.cpp)
    endif (NOT ${ENABLE_TEST_LOGS})

    set(_TEST_LABELS "")
    if(_TEST_ARGS_USE_FS)
        enable_test_filesystem()
    endif()

    target_link_libraries(${_TESTNAME} PRIVATE gtest_main gmock log-api)
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

    if(_TEST_ENTITY)
        add_dependencies(unittests-${_TEST_ENTITY} ${_TESTNAME})
        list(APPEND _TEST_LABELS ${_TEST_ENTITY})
    endif()

    gtest_add_tests(
        TARGET ${_TESTNAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        TEST_LIST _TEST_LIST
    )

    set_tests_properties(${_TEST_LIST} PROPERTIES LABELS ${_TEST_LABELS})
endfunction()

# Compile Catch2 main entry point as a separate cmake target. This way, it is not needed to recompile the whole catch2
# each time something changes inside specific test case.
add_library(catch2_main STATIC catch2_main.cpp)
add_library(Catch2::main ALIAS catch2_main)
target_link_libraries(catch2_main PUBLIC Catch2::Catch2)

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
    if (NOT ${ENABLE_TEST_LOGS})
        if (NOT ${_TESTNAME} STREQUAL "catch2-utils-log" AND NOT ${_TESTNAME} STREQUAL "catch2-utils-logdumps")
            target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-logs.cpp)
            target_sources(${_TESTNAME} PRIVATE ${ROOT_TEST_DIR}/mock-freertos-tls.cpp)
        endif(NOT ${_TESTNAME} STREQUAL "catch2-utils-log" AND NOT ${_TESTNAME} STREQUAL "catch2-utils-logdumps")
    endif (NOT ${ENABLE_TEST_LOGS})

    set(_TEST_LABELS "")
    if(_TEST_ARGS_USE_FS)
        enable_test_filesystem()
    endif()

    target_link_libraries(${_TESTNAME} PRIVATE Catch2::main log-api)
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

function(add_test_entity)
    cmake_parse_arguments(
        _ARGS
        ""
        "NAME"
        ""
        ${ARGN}
    )

    if(NOT _ARGS_NAME)
        message(FATAL_ERROR "You must provide a test entity name")
    endif(NOT _ARGS_NAME)

    message("Adding test entity: ${_ARGS_NAME}")

    set_directory_properties(PROPERTIES
        TEST_ENTITY ${_ARGS_NAME}
        LABELS ${_ARGS_NAME}
    )

    add_custom_target(unittests-${_ARGS_NAME})
    add_custom_target(check-${_ARGS_NAME} ${CMAKE_CTEST_COMMAND} -L ${_ARGS_NAME})
    add_dependencies(check-${_ARGS_NAME} unittests-${_ARGS_NAME})
endfunction()

set(_CATCH_DISCOVER_TESTS_SCRIPT ${_CATCH_DISCOVER_TESTS_SCRIPT} PARENT_SCOPE)
