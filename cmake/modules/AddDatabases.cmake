#[[
Example of use:
add_databases_target(
        TARGET <target_name>
        DB_SET <database set dir>
        SOURCE_DIR <source_dir>
        DEST_DIR <destination_dir>
        DEVEL <true/false>
        DEPENDS <dependencies list>
)
]]
#
function(add_databases_target)
    cmake_parse_arguments(
            _ARG
            ""
            "TARGET;COMMON_DIR;PRODUCT_DIR;DB_SET;DEST_DIR;DEVEL;"
            "DEPENDS"
            ${ARGN}
    )

    if (${_ARG_DEVEL} STREQUAL "ON" OR ${_ARG_DEVEL} EQUAL 1)
        set(DEVEL --devel True)
    endif ()

    add_custom_target(
            ${_ARG_TARGET}
            DEPENDS ${_ARG_DEPENDS}

            COMMAND python3 ${PROJECT_SOURCE_DIR}/tools/db_migration.py init
            --env ${CMAKE_BINARY_DIR}
            --dbset ${_ARG_DB_SET}
            --out ${_ARG_DEST_DIR}
            --dirs ${_ARG_PRODUCT_DIR}
            --dirs ${_ARG_COMMON_DIR}

            COMMAND python3 ${PROJECT_SOURCE_DIR}/tools/db_migration.py install
            --env ${CMAKE_BINARY_DIR} ${DEVEL}
    )
endfunction()