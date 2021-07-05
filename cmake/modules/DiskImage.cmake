function(add_image SOURCE_TARGET)
    set(DISK_IMAGE_NAME ${SOURCE_TARGET}.img)
    set(DISK_IMAGE ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})
    set(BIN_FILE_TARGET ${SOURCE_TARGET}-boot.bin)
    get_target_property(BIN_FILE ${BIN_FILE_TARGET} BIN_FILE)

    if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        SET(TARGET_DEP ${BIN_FILE} pureflash)
    else()
        SET(TARGET_DEP ${SOURCE_TARGET})
    endif()

    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS genlittlefs
        DEPENDS assets
        DEPENDS ${BIN_FILE_TARGET}
        DEPENDS ${TARGET_DEP}
        DEPENDS ${CMAKE_SOURCE_DIR}/generate_image.sh
        COMMAND ${CMAKE_SOURCE_DIR}/generate_image.sh
        ${DISK_IMAGE}
        ${CMAKE_BINARY_DIR}
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:${BIN_FILE}>"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME}"
        )

    message("Adding disk image target: ${DISK_IMAGE_NAME}")
    add_custom_target(${SOURCE_TARGET}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})

endfunction()
