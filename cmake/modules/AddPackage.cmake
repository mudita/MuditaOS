if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_COMPONENTS_ALL Standalone)
else()
    set(CPACK_SYSTEM_NAME "RT1051")
    set(CPACK_COMPONENTS_ALL Standalone Update)
endif()

macro(set_cpack_vars)

    set(CPACK_PACKAGE_VENDOR "Mudita")
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://mudita.com/products/pure/")

    set(CPACK_TOPLEVEL_TAG ${CPACK_SYSTEM_NAME})
    set(CPACK_OUTPUT_CONFIG_FILE ${CMAKE_BINARY_DIR}/PackageConfig.cmake)
    set(CPACK_GENERATOR "External")
    set(CPACK_COMPONENTS_GROUPING IGNORE)
    set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)
    set(PACKAGE_COMMON_NAME ${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_TOPLEVEL_TAG})
    set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_COMMON_NAME})
    set(PACKAGE_STAGING_DIRECTORY ${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_TOPLEVEL_TAG}/${CPACK_GENERATOR}/${CPACK_PACKAGE_FILE_NAME})

#    message("---cpak vars---")
#    print_var(PACKAGE_COMMON_NAME)
#    print_var(CPACK_PACKAGE_NAME)
#    print_var(CPACK_PACKAGE_FILE_NAME)
#    print_var(CPACK_TOPLEVEL_TAG)
#    print_var(CPACK_GENERATOR)
#    print_var(PACKAGE_STAGING_DIRECTORY)
#    message("----------------")
endmacro()

function(add_standalone_image SOURCE_TARGET)
    if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        set(BIN_FILE ${SOURCE_TARGET}-boot.bin)
        set(CPACK_PACKAGE_NAME ${SOURCE_TARGET})
        set_cpack_vars()

        set(ECOBOOT_FILE ecoboot.bin)
        set(VERSION_JSON_FILE version.json)

        set(PACKAGE_STANDALONE_MIME "application/x-xz")
        set(PACKAGE_STANDALONE_FILE_NAME ${PACKAGE_COMMON_NAME}-image.tar.xz)

        add_custom_target(${PACKAGE_COMMON_NAME}-package-standalone
            COMMAND tar -ScJf ${PACKAGE_STANDALONE_FILE_NAME} ${SOURCE_TARGET}.img
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS ${BIN_FILE} ${ECOBOOT_FILE}-target ${VERSION_JSON_FILE}-target ${SOURCE_TARGET}.img
            )

        message("Adding stand alone target '${SOURCE_TARGET}-StandaloneImage'")
        add_custom_target(${SOURCE_TARGET}-StandaloneImage
            DEPENDS ${PACKAGE_COMMON_NAME}-package-standalone)
    endif()
endfunction()


function(add_update_package SOURCE_TARGET)
    if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        set(CPACK_PACKAGE_NAME ${SOURCE_TARGET})
        set_cpack_vars()
        set(UPDATE_PKG "${SOURCE_TARGET}-${CMAKE_PROJECT_VERSION}-${CPACK_SYSTEM_NAME}-Update.tar")
        add_custom_command(
            OUTPUT ${UPDATE_PKG}
            DEPENDS ${SOURCE_TARGET}
            DEPENDS ${SOURCE_TARGET}-boot.bin
            DEPENDS ecoboot.bin-target
            DEPENDS version.json-target
            DEPENDS assets
            COMMAND ${CMAKE_SOURCE_DIR}/tools/generate_update_image.sh ${SOURCE_TARGET} ${CMAKE_PROJECT_VERSION} ${CPACK_SYSTEM_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating update image"
            )
        message("Adding '${SOURCE_TARGET}-UpdatePackage' target")
        add_custom_target(${SOURCE_TARGET}-UpdatePackage
            DEPENDS ${UPDATE_PKG}
            )
    endif()
endfunction()
