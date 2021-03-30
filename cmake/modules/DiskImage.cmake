set(DISK_IMAGE_NAME ${CMAKE_PROJECT_NAME}.img)
set(DISK_IMAGE ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    SET(TARGET_DEP ${BIN_FILE} pureflash)
else()
    SET(TARGET_DEP ${CMAKE_PROJECT_NAME})
endif()

add_custom_target(
    disk_image ALL
    DEPENDS genlittlefs
    DEPENDS assets
    DEPENDS ${TARGET_DEP}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_purephone_image.sh
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate_purephone_image.sh ${DISK_IMAGE} ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generate ${DISK_IMAGE}"
    )
