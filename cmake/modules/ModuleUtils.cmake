macro(module_is_test_entity)
    if(${ENABLE_TESTS})
        if (NOT ${ARGV0} STREQUAL "")
            set(_NAME ${ARGV0})
        else()
            string(REGEX MATCH "module-\([^/]*\)" _NAME ${CMAKE_CURRENT_LIST_DIR})
            set(_NAME ${CMAKE_MATCH_1})
        endif()

        if(${_NAME} STREQUAL "")
            message(FATAL_ERROR "Can't determine module name to enable testing.")
        endif()

        add_test_entity(NAME ${_NAME})

        enable_entity_coverage(
            NAME
                ${_NAME}
            COVERAGE_PATH
                .*/module-${_NAME}/.*
        )
    endif()
endmacro()
