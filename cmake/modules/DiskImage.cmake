function(add_image SOURCE_TARGET)
    set(DISK_IMAGE_NAME ${SOURCE_TARGET}.img)
    set(DISK_IMAGE ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})
    set(BIN_FILE ${SOURCE_TARGET}-boot.bin)

    if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        SET(TARGET_DEP ${BIN_FILE} pureflash)
    else()
        SET(TARGET_DEP ${SOURCE_TARGET})
    endif()

    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS
        DEPENDS genlittlefs
        DEPENDS assets
        DEPENDS ${TARGET_DEP}
        DEPENDS ${BIN_FILE}
        DEPENDS ${CMAKE_SOURCE_DIR}/generate_purephone_image.sh
        COMMAND ${CMAKE_SOURCE_DIR}/generate_purephone_image.sh ${DISK_IMAGE} ${CMAKE_BINARY_DIR} ${BIN_FILE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME}"
        )
    message("Adding disk image target: ${DISK_IMAGE_NAME}")
    add_custom_target(${SOURCE_TARGET}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})
endfunction()
