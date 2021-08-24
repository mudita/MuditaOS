function(add_version_json SOURCE_TARGET)
    add_custom_command(OUTPUT ${SOURCE_TARGET}-version.json
        COMMAND ${CMAKE_COMMAND}
            -DSRC_DIR=${CMAKE_SOURCE_DIR}
            -DSRC_FILE=${CMAKE_SOURCE_DIR}/config/version.json.cmake_template
            -DDST_FILE=${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            -DBOOTLOADER_FILENAME=ecoboot.bin
            -DBOOTLOADER_FILE=${CMAKE_BINARY_DIR}/ecoboot.bin
            -DBOOTLOADER_VERSION_FILE=${CMAKE_BINARY_DIR}/ecoboot.version
            -DBOOT_FILENAME=boot.bin
            -DBOOT_FILE=$<TARGET_PROPERTY:${SOURCE_TARGET},BIN_FILE>
            -DBOOT_VERSION=${CMAKE_PROJECT_VERSION}
            -DUPDATER_FILENAME=updater.bin
            -DUPDATER_FILE=${CMAKE_BINARY_DIR}/updater.bin
            -DUPDATER_VERSION_FILE=${CMAKE_BINARY_DIR}/PureUpdater.version
            -B ${CMAKE_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/modules/ConfigureVersionJson.cmake
        DEPENDS ecoboot.bin-target
        DEPENDS updater.bin-target
        DEPENDS ${SOURCE_TARGET}-boot.bin
    )

    add_custom_target(${SOURCE_TARGET}-version.json-target DEPENDS ${SOURCE_TARGET}-version.json)

    multicomp_install(FILES ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json DESTINATION "./" RENAME "version.json" COMPONENTS Standalone Update)
endfunction()
