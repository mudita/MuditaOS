function(add_image IMAGE_NAME SYSROOT ASSETS)

    set(DISK_IMAGE_NAME ${IMAGE_NAME}.img)
    set(DISK_IMAGE ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})
    
    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS genlittlefs
        DEPENDS ${ASSETS}
        DEPENDS ${CMAKE_SOURCE_DIR}/generate_image.sh
        COMMAND
            ${CMAKE_SOURCE_DIR}/generate_image.sh
            ${DISK_IMAGE}
            ${CMAKE_BINARY_DIR}/${SYSROOT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME}"
    )

    message("Adding disk image target: ${DISK_IMAGE_NAME}")

    add_custom_target(${IMAGE_NAME}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})

endfunction()
