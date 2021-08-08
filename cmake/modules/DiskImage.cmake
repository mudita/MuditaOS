function(add_image)
    set(PRODUCT ${ARGV0})

    set(SCRIPT_PATH ${CMAKE_SOURCE_DIR}/tools/generate_image.sh)

    set(DISK_IMAGE_NAME ${PRODUCT}.img)
    set(DISK_IMAGE_PATH ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})

    set_target_properties(
            ${PRODUCT}
        PROPERTIES
            DISK_IMAGE ${DISK_IMAGE_PATH}
    )

    set(COMMAND_DEPENDS "genlittlefs")
    list(APPEND COMMNDS_DEPENDS ${SCRIPT_PATH})
    list(APPEND COMMAND_DEPENDS ${PRODUCT}-staging)

    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS ${COMMAND_DEPENDS}
        COMMAND
            ${SCRIPT_PATH}
            ${DISK_IMAGE_NAME}
            $<TARGET_PROPERTY:${PRODUCT},SYSROOT>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME}"
    )

    message("Adding disk image target: ${DISK_IMAGE_NAME}")

    add_custom_target(${PRODUCT}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})

endfunction()

