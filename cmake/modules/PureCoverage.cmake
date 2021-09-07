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
        .*/third-party/.*
        board/linux/libiosyscalls/.*
        host-tools/.*
        module-audio/Audio/decoder/dr_flac.h
        module-bluetooth/Bluetooth/glucode/.*
        module-bluetooth/lib/.*
        module-db/Database/sqlite3.c
        module-lwip/lib/.*
        module-os/.*
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

function(enable_entity_coverage)
    if(COVERAGE_ENABLE)
        cmake_parse_arguments(
            _ARGS
            ""
            "NAME;COVERAGE_PATH"
            ""
            ${ARGN}
        )

        message("Enabling coverage reporting for test entity: ${_ARGS_NAME} (${PROJECT_NAME})")

        if(_ARGS_COVERAGE_PATH)
            set(GCOVR_ADDITIONAL_ARGS ${GCOVR_ADDITIONAL_ARGS} -f ${_ARGS_COVERAGE_PATH})
        endif()

        setup_target_for_coverage_gcovr_html(
            NAME coverage-${_ARGS_NAME}-html
            EXECUTABLE ;
            DEPENDENCIES ""
        )

        setup_target_for_coverage_gcovr_xml(
            NAME coverage-${_ARGS_NAME}
            EXECUTABLE ;
            DEPENDENCIES ""
        )

        add_dependencies(coverage-${_ARGS_NAME}-html check-${_ARGS_NAME})
        add_dependencies(coverage-${_ARGS_NAME} check-${_ARGS_NAME})
        add_dependencies(check-${_ARGS_NAME} coverage-cleanup)
    endif()
endfunction(enable_entity_coverage)
