function(add_version_rt1051_json SOURCE_TARGET)
    add_custom_command(OUTPUT ${SOURCE_TARGET}-version.json
        COMMAND ${CMAKE_COMMAND}
            -DSRC_DIR=${CMAKE_SOURCE_DIR}
            -DSRC_FILE=${CMAKE_SOURCE_DIR}/config/version_rt1051.json.cmake_template
            -DDST_FILE=${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            -DBOOTLOADER_FILENAME=ecoboot.bin
            -DBOOTLOADER_FILE=${SYSROOT_BIN_PATH}/ecoboot.bin
            -DOS_FILENAME=os.bin
            -DOS_FILE=$<TARGET_PROPERTY:${SOURCE_TARGET},BIN_FILE>
            -DOS_VERSION=${PROJECT_VERSION}
            -DRECOVERY_FILENAME=recovery.bin
            -DRECOVERY_FILE=${SYSROOT_BIN_PATH}/recovery.bin
            -DOS_VERSION_MAJOR=${OS_VERSION_MAJOR}
            -DOS_VERSION_MINOR=${OS_VERSION_MINOR}
            -DOS_VERSION_PATCH=${OS_VERSION_PATCH}
            -DOS_VERSION_LABEL=${OS_VERSION_LABEL}
            -DECOBOOT_BIN_VERSION=${ECOBOOT_BIN_VERSION}
            -DRECOVERY_BIN_VERSION=${RECOVERY_BIN_VERSION}
            -B ${CMAKE_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/modules/ConfigureVersionJson.cmake
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/add_dbs_to_version_json.py
            --input_path ${CMAKE_SOURCE_DIR}/products/${SOURCE_TARGET}/services/db/databases/databases.json
            --output_path ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            --product ${SOURCE_TARGET}
        DEPENDS ecoboot.bin-target
        DEPENDS recovery.bin-target
        DEPENDS ${SOURCE_TARGET}-boot.bin
    )

    add_custom_command(OUTPUT add-${SOURCE_TARGET}-version.json
        COMMAND mkdir -p ${SYSROOT_PATH}/system_a
        COMMAND rsync -qau
            ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            ${SYSROOT_PATH}/system_a/version.json
        DEPENDS
            ${SOURCE_TARGET}-version.json
    )

    add_custom_target(${SOURCE_TARGET}-version.json-target DEPENDS add-${SOURCE_TARGET}-version.json)

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
COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/add_dbs_to_version_json.py
            --input_path ${CMAKE_SOURCE_DIR}/products/${SOURCE_TARGET}/services/db/databases/databases.json
            --output_path ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            --product ${SOURCE_TARGET}
    )

    add_custom_command(OUTPUT add-${SOURCE_TARGET}-version.json
        COMMAND mkdir -p ${SYSROOT_PATH}/system_a
        COMMAND rsync -qau
            ${CMAKE_BINARY_DIR}/${SOURCE_TARGET}-version.json
            ${SYSROOT_PATH}/system_a/version.json
        DEPENDS
            ${SOURCE_TARGET}-version.json
    )

    add_custom_target(${SOURCE_TARGET}-version.json-target DEPENDS add-${SOURCE_TARGET}-version.json)

endfunction()
