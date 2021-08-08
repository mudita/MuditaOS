macro(_setup_dirs)
    set(ARTIFACTS_DIR ${CMAKE_SOURCE_DIR}/image)

    get_target_property(SYSROOT ${TARGET} SYSROOT)
    set(SYS_DIR "${SYSROOT}/sys")
    set(CURRENT_DIR "${SYS_DIR}/current")
    set(USER_DIR "${SYS_DIR}/user")

    set(ASSETS_SRC_DIR ${ARTIFACTS_DIR}/assets)
    set(ASSETS_DST_DIR ${CURRENT_DIR}/assets)
endmacro()

function(_stage_prepare)
    set(TARGET ${ARGV0})
    _setup_dirs()

    add_custom_target(
            ${TARGET}-stage-prepare
        COMMENT "Prepare package stage for ${TARGET}."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SYS_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CURRENT_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${USER_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSETS_DST_DIR}
    )
endfunction()

function(_stage_common_files)
    set(TARGET ${ARGV0})
    _setup_dirs()

    add_custom_target(
            ${TARGET}-stage-common-files
        COMMENT "Stage common files for ${TARGET}."
        DEPENDS ${TARGET}-stage-prepare
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARTIFACTS_DIR}/.boot.json ${SYS_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARTIFACTS_DIR}/.boot.json.crc32 ${SYS_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARTIFACTS_DIR}/personalization.json ${SYS_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARTIFACTS_DIR}/user ${USER_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARTIFACTS_DIR}/Luts.bin ${CURRENT_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARTIFACTS_DIR}/country-codes.db ${CURRENT_DIR}
    )

    add_dependencies(${TARGET}-staging ${TARGET}-stage-common-files)
endfunction()

function(_stage_assets)
    set(TARGET ${ARGV0})
    get_target_property(ASSETS ${TARGET} ASSETS)
    _setup_dirs()

    foreach(ASSET ${ASSETS})
        set(ASSET_SRC "${ASSETS_SRC_DIR}/${ASSET}")
        set(ASSET_DST "${ASSETS_DST_DIR}/${ASSET}")

        if(NOT EXISTS "${ASSET_SRC}")
            message(FATAL_ERROR "target_assets(${TARGET}): asset '${ASSET}' not found in '${ASSETS_SRC_DIR}'.")
        endif()

        list(APPEND ASSETS_DST_FILES "${ASSET_DST}")
        add_custom_command(
            OUTPUT ${ASSET_DST}
            COMMENT "Stage ${ASSET} for ${TARGET}."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_SRC} ${ASSET_DST}
        )
    endforeach()

    add_custom_target(
            ${TARGET}-stage-assets
        COMMENT "Stage assets for ${TARGET}."
        DEPENDS ${TARGET}-stage-prepare
        DEPENDS ${ASSETS_DST_FILES}
    )

    add_dependencies(${TARGET}-staging ${TARGET}-stage-assets)
endfunction()

function(_stage_boot_bin)
    set(TARGET ${ARGV0})
    get_target_property(PACKAGE_BOOT_BIN ${TARGET} PACKAGE_BOOT_BIN)

    if(NOT ${PACKAGE_BOOT_BIN})
        return()
    endif()

    add_custom_target(
            ${TARGET}-stage-boot-bin
        COMMENT "Stage boot.bin for ${TARGET}."
        DEPENDS ${TARGET}-stage-prepare
        DEPENDS ${TARGET}-boot.bin
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:${TARGET},BIN_FILE> ${CURRENT_DIR}
    )

    add_dependencies(${TARGET}-staging ${TARGET}-stage-boot-bin)
endfunction()

function(_stage_updater_bin)
    set(TARGET ${ARGV0})
    get_target_property(PACKAGE_UPDATER_BIN ${TARGET} PACKAGE_UPDATER_BIN)

    if(NOT ${PACKAGE_UPDATER_BIN})
        return()
    endif()

    add_custom_target(
            ${TARGET}-stage-updater-bin
        COMMENT "Stage updater.bin for ${TARGET}."
        DEPENDS ${TARGET}-stage-prepare
        DEPENDS updater.bin-target
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:updater.bin-target,BIN_FILE> ${CURRENT_DIR}
    )

    add_dependencies(${TARGET}-staging ${TARGET}-stage-updater-bin)
endfunction()

function(stage_product)
    set(TARGET ${ARGV0})
    _setup_dirs()

    if(${SYSROOT} STREQUAL "")
        message(FATAL_ERROR "Staging SYSROOT not specified for target: ${TARGET}")
    endif()

    add_custom_target(${TARGET}-staging)

    _stage_prepare(${TARGET})
    _stage_common_files(${TARGET})
    _stage_assets(${TARGET})
    _stage_boot_bin(${TARGET})
    _stage_updater_bin(${TARGET})
endfunction()
