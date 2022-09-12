#[[
Example of use:
add_directories(
        TARGET <target name>
        PREFIX <paths prefix>
        DEPENDS <dependencies list>
        DIRECTORIES <dir1 dir2 dir3 dir_n>
)
]]#
function(add_directories)
    cmake_parse_arguments(
            _ASSETS
            ""
            "TARGET;PREFIX"
            "DEPENDS;DIRECTORIES"
            ${ARGN}
    )
    set(command)
    foreach (entry ${_ASSETS_DIRECTORIES})
        list(APPEND command
                COMMAND mkdir -p ${_ASSETS_PREFIX}/${entry})
    endforeach ()

    add_custom_target(
            ${_ASSETS_TARGET}
            DEPENDS ${_ASSETS_DEPENDS}
            ${command}
            COMMENT
            "Adding directories: ${_ASSETS_DIRECTORIES}"
    )
endfunction()