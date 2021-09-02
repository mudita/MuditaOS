function(fetch_ecoboot)
    set(ECOBOOT_ASSET_NAME ecoboot.bin)
    if(${BOARD} STREQUAL "puretx" AND ${BOARD_REVISION} LESS_EQUAL 6)
        set(ECOBOOT_ASSET_NAME ecoboot_T6.bin)
    endif()

    add_custom_command(OUTPUT ecoboot.bin
        COMMAND ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            "$<$<BOOL:$ENV{ASSETS_LOGIN}>:-l$ENV{ASSETS_LOGIN}>"
            "$<$<BOOL:$ENV{ASSETS_TOKEN}>:-t$ENV{ASSETS_TOKEN}>"
            -w ${CMAKE_BINARY_DIR}
            ecoboot download
            -n ${ECOBOOT_ASSET_NAME}
            -o ecoboot.bin
        COMMENT "Downloading ecoboot.bin"
        BYPRODUCTS ecoboot.version
    )

    add_custom_target(ecoboot.bin-target DEPENDS ecoboot.bin)

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/ecoboot.bin DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()
