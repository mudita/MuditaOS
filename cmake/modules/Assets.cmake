function(target_assets)
    set(TARGET ${ARGV0})
    set(options PACKAGE_BOOT_BIN PACKAGE_UPDATER_BIN)
    set(oneValueArgs SYSROOT)
    set(multiValueArgs ASSETS)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")
    message("target_assets() args: '${ARGV}'")

    if(${ARG_PACKAGE_BOOT_BIN})
        set_target_properties(
                ${TARGET}
            PROPERTIES
                PACKAGE_BOOT_BIN ${ARG_PACKAGE_BOOT_BIN}
        )
    endif()

    if(${ARG_PACKAGE_UPDATER_BIN})
        set_target_properties(
                ${TARGET}
            PROPERTIES
                PACKAGE_UPDATER_BIN ${ARG_PACKAGE_UPDATER_BIN}
        )
    endif()

    if(NOT ${ARG_SYSROOT} STREQUAL "")
        set_target_properties(
                ${TARGET}
            PROPERTIES
                SYSROOT ${ARG_SYSROOT}
        )
    endif()

    if(NOT "${ARG_ASSETS}" STREQUAL "")
        set_target_properties(
                ${TARGET}
            PROPERTIES
                ASSETS "${ARG_ASSETS}"
        )
    endif()
endfunction()
