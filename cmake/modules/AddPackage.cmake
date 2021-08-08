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
    set(PACKAGE_COMMON_NAME ${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_TOPLEVEL_TAG})
    set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_COMMON_NAME})
    set(PACKAGE_STAGING_DIRECTORY ${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_TOPLEVEL_TAG}/${CPACK_GENERATOR}/${CPACK_PACKAGE_FILE_NAME})

endmacro()

function(add_standalone_package SOURCE_TARGET)
    if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        return()
    endif()
    set(BIN_FILE ${SOURCE_TARGET}-boot.bin)
    set(CPACK_PACKAGE_NAME ${SOURCE_TARGET})
    set_cpack_vars()

    set(STANDALONE_PKG ${PACKAGE_COMMON_NAME}-image.tar.xz)
    set(PACKAGE_STANDALONE_FILE_NAME ${STANDALONE_PKG} PARENT_SCOPE)
    set(PACKAGE_STANDALONE_MIME "application/x-xz" PARENT_SCOPE)

    add_custom_command(
        OUTPUT ${STANDALONE_PKG}
        COMMAND tar -ScJf ${STANDALONE_PKG} $<TARGET_PROPERTY:${SOURCE_TARGET},DISK_IMAGE>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${SOURCE_TARGET}-disk-img
    )

    add_custom_target(${SOURCE_TARGET}-StandalonePackage
        DEPENDS ${STANDALONE_PKG})
endfunction()


function(add_update_package SOURCE_TARGET)
    if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        return()
    endif()
    set(CPACK_PACKAGE_NAME ${SOURCE_TARGET})
    set_cpack_vars()
    set(UPDATE_PKG "${SOURCE_TARGET}-${CMAKE_PROJECT_VERSION}-${CPACK_SYSTEM_NAME}-Update.tar")

    set(PACKAGE_UPDATE_FILE_NAME ${UPDATE_PKG} PARENT_SCOPE)
    set(PACKAGE_UPDATE_MIME "application/x-tar" PARENT_SCOPE)

    add_custom_command(
        OUTPUT ${UPDATE_PKG}
        DEPENDS ${SOURCE_TARGET}-staging
        COMMAND ${CMAKE_SOURCE_DIR}/tools/generate_update_image.sh ${SOURCE_TARGET} ${CMAKE_PROJECT_VERSION} ${CPACK_SYSTEM_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating update image"
    )

    add_custom_target(${SOURCE_TARGET}-UpdatePackage
        DEPENDS ${UPDATE_PKG}
    )
endfunction()
