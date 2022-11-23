#[[
Example of use:
add_databases_target(
        TARGET <target_name>
        SOURCE_DIR <source_dir>
        DEST_DIR <destination_dir>
        DEVEL <true/false>
        DEPENDS <dependencies list>
)
]]#
function(add_databases_target)
    cmake_parse_arguments(
            _ARG
            ""
            "TARGET;SOURCE_DIR;DEST_DIR;DEVEL;"
            "DEPENDS"
            ${ARGN}
    )

    if (${_ARG_DEVEL} STREQUAL "ON" OR ${_ARG_DEVEL} EQUAL 1)
        set (DEVEL --development True)
    endif()

    add_custom_target(
            ${_ARG_TARGET}
            DEPENDS ${_ARG_DEPENDS}

            COMMAND python3 ${PROJECT_SOURCE_DIR}/tools/init_databases.py --input_path ${_ARG_SOURCE_DIR} --output_path ${_ARG_DEST_DIR} ${DEVEL}
            COMMENT
            "Creating databases using schemas from: ${_ARG_SOURCE_DIR} and storing them under: ${_ARG_DEST_DIR}"
    )
endfunction()