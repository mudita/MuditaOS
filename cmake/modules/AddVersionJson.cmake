function(add_version_rt1051_json SOURCE_TARGET)
    add_custom_command(OUTPUT ${SOURCE_TARGET}-version.json
        COMMAND ${CMAKE_COMMAND}
            -DSRC_DIR=${CMAKE_SOURCE_DIR}
            -DSRC_FILE=${CMAKE_SOURCE_DIR}/config/version_rt1051.json.cmake_template
            -DDST_FILE=${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            -DBOOTLOADER_FILENAME=ecoboot.bin
            -DBOOTLOADER_FILE=${CMAKE_BINARY_DIR}/ecoboot.bin
            -DBOOT_FILENAME=boot.bin
            -DBOOT_FILE=$<TARGET_PROPERTY:${SOURCE_TARGET},BIN_FILE>
            -DBOOT_VERSION=${PROJECT_VERSION}
            -DUPDATER_FILENAME=updater.bin
            -DUPDATER_FILE=${CMAKE_BINARY_DIR}/updater.bin
            -DOS_VERSION_MAJOR=${OS_VERSION_MAJOR}
            -DOS_VERSION_MINOR=${OS_VERSION_MINOR}
            -DOS_VERSION_PATCH=${OS_VERSION_PATCH}
            -DOS_VERSION_LABEL=${OS_VERSION_LABEL}
            -DECOBOOT_BIN_VERSION=${ECOBOOT_BIN_VERSION}
            -DUPDATER_BIN_VERSION=${UPDATER_BIN_VERSION}
            -B ${CMAKE_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/modules/ConfigureVersionJson.cmake
        DEPENDS ecoboot.bin-target
        DEPENDS updater.bin-target
        DEPENDS ${SOURCE_TARGET}-boot.bin
    )

    add_custom_target(${SOURCE_TARGET}-version.json-target DEPENDS ${SOURCE_TARGET}-version.json)

    multicomp_install(FILES ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json DESTINATION "./" RENAME "version.json" COMPONENTS Standalone Update)
endfunction()

function(add_version_linux_json SOURCE_TARGET)
    add_custom_command(OUTPUT ${SOURCE_TARGET}-version.json
        COMMAND ${CMAKE_COMMAND}
            -DSRC_DIR=${CMAKE_SOURCE_DIR}
            -DSRC_FILE=${CMAKE_SOURCE_DIR}/config/version_linux.json.cmake_template
            -DDST_FILE=${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            -DOS_VERSION_MAJOR=${OS_VERSION_MAJOR}
            -DOS_VERSION_MINOR=${OS_VERSION_MINOR}
            -DOS_VERSION_PATCH=${OS_VERSION_PATCH}
            -DOS_VERSION_LABEL=${OS_VERSION_LABEL}
            -B ${CMAKE_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/modules/ConfigureVersionJson.cmake
    )

    add_custom_target(${SOURCE_TARGET}-version.json-target DEPENDS ${SOURCE_TARGET}-version.json)

    multicomp_install(FILES ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json DESTINATION "./" RENAME "version.json" COMPONENTS Standalone Update)
endfunction()
