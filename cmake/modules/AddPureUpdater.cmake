function(copy_updater_bin)
    set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/image)
    set(UPDATER_ASSET_BIN PureUpdater_RT.bin)

    add_custom_command(OUTPUT updater.bin
        COMMAND cp
            ${ASSETS_SOURCE_DIR}/${UPDATER_ASSET_BIN}
            ${CMAKE_BINARY_DIR}/updater.bin
        COMMENT "Copying PureUpdater_RT.bin"
    )

    add_custom_target(updater.bin-target DEPENDS updater.bin)

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/updater.bin DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()


function(copy_updater_ver)
    set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/image)
    set(UPDATER_ASSET_VER PureUpdater.version)

    add_custom_command(OUTPUT PureUpdater.version
        COMMAND cp
            ${ASSETS_SOURCE_DIR}/${UPDATER_ASSET_VER}
            ${CMAKE_BINARY_DIR}/PureUpdater.version
        COMMENT "Downloading PureUpdater.version"
    )

    add_custom_target(updater.ver-target DEPENDS PureUpdater.version)

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/PureUpdater.version DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()
