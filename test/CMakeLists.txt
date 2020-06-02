if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
   add_subdirectory(googletest)
endif()
include(GoogleTest)

function(add_gtest_executable TESTNAME TESTSRCS)
    add_executable(${TESTNAME} EXCLUDE_FROM_ALL ${TESTSRCS})

    target_link_libraries(${TESTNAME} gtest_main gmock)
    target_compile_options(${TESTNAME} PUBLIC "-pthread")
    target_link_options(${TESTNAME} PUBLIC "-pthread")

    add_dependencies(check ${TESTNAME})
    gtest_add_tests(${TESTNAME} "" AUTO)
endfunction()
