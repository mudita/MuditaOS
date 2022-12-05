macro(set_cpack_vars)
    if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
        set(CPACK_SYSTEM_NAME "Linux")
        set(CPACK_COMPONENTS_ALL Standalone)
    elseif(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        set(CPACK_SYSTEM_NAME "RT1051")
        set(CPACK_COMPONENTS_ALL Standalone Update)
    else()
        message(FATAL_ERROR "Unsupported project target: '${PROJECT_TARGET}'")
    endif()

    set(CPACK_PACKAGE_VENDOR "Mudita")
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://mudita.com/products/pure/")

    set(CPACK_TOPLEVEL_TAG ${CPACK_SYSTEM_NAME})
    set(CPACK_OUTPUT_CONFIG_FILE ${CMAKE_BINARY_DIR}/PackageConfig.cmake)
    set(CPACK_GENERATOR "External")
    set(CPACK_COMPONENTS_GROUPING IGNORE)
    set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)
    set(PACKAGE_COMMON_NAME ${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CPACK_TOPLEVEL_TAG})
    set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_COMMON_NAME})
    set(PACKAGE_STAGING_DIRECTORY ${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_TOPLEVEL_TAG}/${CPACK_GENERATOR}/${CPACK_PACKAGE_FILE_NAME})

endmacro()

function(add_standalone_image SOURCE_TARGET)
    if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        return()
    endif()
    set(BIN_FILE ${SOURCE_TARGET}-boot.bin)
    set(CPACK_PACKAGE_NAME ${SOURCE_TARGET})
    set_cpack_vars()

    set(STANDALONE_PKG ${PACKAGE_COMMON_NAME}-image.tar.xz)
    set(PACKAGE_STANDALONE_FILE_NAME ${STANDALONE_PKG} PARENT_SCOPE)
    set(PACKAGE_STANDALONE_MIME "application/x-xz" PARENT_SCOPE)

    add_custom_target(${PACKAGE_COMMON_NAME}-package-standalone
        # please do not:
        # 1. change compression to -9 (or higher) as i will have detrimental effects on compression times with not much gain
        # 2. change -T parameter to explicit thread count - xz with T0 will adjust thread count to your machine capatibilies
        COMMAND tar -I 'xz -T0' -Scf ${STANDALONE_PKG} ${SOURCE_TARGET}.img
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${BIN_FILE}
        DEPENDS json-common-target
        DEPENDS json-proprietary-target
        DEPENDS json-rt1051-target
        DEPENDS ecoboot.bin-target
        DEPENDS recovery.bin-target
        DEPENDS ${SOURCE_TARGET}-version.json-target
        DEPENDS ${SOURCE_TARGET}.img
        )

    message("Adding stand alone target '${SOURCE_TARGET}-StandaloneImage'")
    add_custom_target(${SOURCE_TARGET}-StandaloneImage
        DEPENDS ${PACKAGE_COMMON_NAME}-package-standalone)
endfunction()


function(add_update_package SOURCE_TARGET)
    if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        return()
    endif()
    set(CPACK_PACKAGE_NAME ${SOURCE_TARGET})
    set_cpack_vars()
    set(UPDATE_PKG "${SOURCE_TARGET}-${PROJECT_VERSION}-RT1051-Update.tar")

    set(PACKAGE_UPDATE_FILE_NAME ${UPDATE_PKG} PARENT_SCOPE)
    set(PACKAGE_UPDATE_MIME "application/x-tar" PARENT_SCOPE)

    add_custom_command(
        OUTPUT ${UPDATE_PKG}
        DEPENDS ${SOURCE_TARGET}
                ${SOURCE_TARGET}-boot.bin
                ${SOURCE_TARGET}-version.json-target
                ecoboot.bin-target
                recovery.bin-target
                assets
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/generate_update_package.py --output_path ${CMAKE_BINARY_DIR} --system_path ${SYSROOT_PATH}/system_a --product ${SOURCE_TARGET}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating update image: ${UPDATE_PKG}"
    )

    add_custom_command(
        OUTPUT ${UPDATE_PKG}.sig
        DEPENDS ${UPDATE_PKG}
        COMMAND python3 ${SIGN_CLIENT_PATH}/signclient.py --sha256 ${UPDATE_PKG} --out_file ${UPDATE_PKG}.sig --keystore ${KEYSTORE} --keyslot ${KEYSLOT} --server ${SERVER} --login ${LOGIN}
        COMMENT "Generating update signature"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    message("Adding '${SOURCE_TARGET}-UpdatePackage' target")
    if(ENABLE_SECURE_BOOT)
        add_custom_target(${SOURCE_TARGET}-UpdatePackage
            DEPENDS ${UPDATE_PKG}.sig
        )
    else()
        add_custom_target(${SOURCE_TARGET}-UpdatePackage
            DEPENDS ${UPDATE_PKG}
        )
    endif()
endfunction()
