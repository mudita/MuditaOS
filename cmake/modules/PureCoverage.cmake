option(COVERAGE_ENABLE "Enable code coverage report generation")

if(COVERAGE_ENABLE)
    if(NOT (${PROJECT_TARGET_NAME} STREQUAL "linux" AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
        message(FATAL_ERROR "Coverage generation is supported for the Linux/Debug configuration only")
    endif()

    include(CodeCoverage)
    include(ProcessorCount)

    append_coverage_compiler_flags()

    ProcessorCount(N)
    if (NOT N EQUAL 0)
        set(COVERAGE_PARALLEL_JOBS -j ${N})
    endif()

    set(COVERAGE_EXCLUDES
        .*/test/.*
        .*/tests/.*
        .*/thirdparty/.*
        board/linux/libiosyscalls/.*
        host-tools/.*
        module-audio/Audio/decoder/dr_flac.h
        module-audio/Audio/decoder/minimp3/minimp3.h
        module-bluetooth/Bluetooth/glucode/.*
        module-bluetooth/lib/.*
        module-db/Database/sqlite3.c
        module-lwip/lib/.*
        module-os/.*
        module-utils/date/include/date/date.h
        module-utils/gsl/.*
        module-utils/json/.*
        module-utils/libphonenumber/.*
        module-utils/microtar/.*
        module-utils/parallel-hashmap/.*
        module-utils/protobuf/.*
        module-utils/pugixml/.*
        module-utils/re2/.*
        module-utils/segger/.*
        module-utils/sml/.*
        module-utils/taglib/.*
        module-utils/tinyexpr/.*
        source/main.cpp
        test/.*
    )

    SET(GCOVR_ADDITIONAL_ARGS
        --exclude-throw-branches
        --print-summary
        ${COVERAGE_PARALLEL_JOBS}
    )

    # target to cleanup gcda's from the previous run
    add_custom_target(
        coverage-cleanup
        COMMAND find . -name "*.gcda" -delete
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Cleaning up coverage data files"
    )

    # full HTML report with a test run
    setup_target_for_coverage_gcovr_html(
        NAME coverage-all-html
        EXECUTABLE ${CMAKE_CTEST_COMMAND}
        DEPENDENCIES unittests coverage-cleanup
    )

    # html report without running any ctest command
    setup_target_for_coverage_gcovr_html(
        NAME coverage-html
        EXECUTABLE ;
        DEPENDENCIES ""
    )

    # cobertura report without running any ctest command
    setup_target_for_coverage_gcovr_xml(
        NAME coverage
        EXECUTABLE ;
        DEPENDENCIES ""
    )
endif()
