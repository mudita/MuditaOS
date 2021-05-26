# download the bootloader
set(ECOBOOT_FILE ecoboot.bin)
set(VERSION_JSON_FILE version.json)

function(fetch_ecoboot)
    set(ECOBOOT_DOWNLOAD_LOG ${CMAKE_BINARY_DIR}/download_info.txt)
        add_custom_command(OUTPUT ${ECOBOOT_FILE}
            COMMAND ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            "$<$<BOOL:$ENV{ASSETS_LOGIN}>:-l$ENV{ASSETS_LOGIN}>"
            "$<$<BOOL:$ENV{ASSETS_TOKEN}>:-t$ENV{ASSETS_TOKEN}>"
            -w ${CMAKE_BINARY_DIR} ecoboot download
            > ${ECOBOOT_DOWNLOAD_LOG}
            COMMENT "Downloading ${ECOBOOT_FILE}"
            BYPRODUCTS ${ECOBOOT_DOWNLOAD_LOG}
            )

    add_custom_target(${ECOBOOT_FILE}-target DEPENDS ${ECOBOOT_FILE})

    # generate version.json file (runs CMake script at build time)
    add_custom_command(OUTPUT ${VERSION_JSON_FILE}
        COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${CMAKE_SOURCE_DIR}
        -DECOBOOT_DOWNLOAD_LOG=${ECOBOOT_DOWNLOAD_LOG}
        -B ${CMAKE_BINARY_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/modules/GenUpdateVersionJson.cmake
        DEPENDS ${ECOBOOT_FILE}-target ${ECOBOOT_DOWNLOAD_LOG}
        )

    add_custom_target(${VERSION_JSON_FILE}-target DEPENDS ${VERSION_JSON_FILE})

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/${ECOBOOT_FILE} DESTINATION "./"
        COMPONENTS Standalone Update)
    multicomp_install(FILES ${CMAKE_BINARY_DIR}/${VERSION_JSON_FILE} DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()
