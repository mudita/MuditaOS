function(fetch_updater)
    set(UPDATER_ASSET_NAME PureUpdater_RT.bin)

    add_custom_command(OUTPUT updater.bin
        COMMAND ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            "$<$<BOOL:$ENV{ASSETS_LOGIN}>:-l$ENV{ASSETS_LOGIN}>"
            "$<$<BOOL:$ENV{ASSETS_TOKEN}>:-t$ENV{ASSETS_TOKEN}>"
            -w ${CMAKE_BINARY_DIR}
            PureUpdater download
            -n ${UPDATER_ASSET_NAME}
            -o updater.bin
        COMMENT "Downloading updater.bin"
        BYPRODUCTS PureUpdater.version
    )

    add_custom_target(updater.bin-target DEPENDS updater.bin)

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/updater.bin DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()
