set(DISK_IMAGE_NAME ${CMAKE_PROJECT_NAME}.img)
set(DISK_IMAGE ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})

add_custom_command(
    OUTPUT ${DISK_IMAGE}
    DEPENDS genlittlefs
    DEPENDS assets
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_purephone_image.sh
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate_purephone_image.sh ${DISK_IMAGE} ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generate ${DISK_IMAGE}"
    )

add_custom_target(
    disk_image 
    DEPENDS ${DISK_IMAGE}
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_dependencies(disk_image ${CMAKE_PROJECT_NAME})
    add_dependencies(check disk_image)
else()
    message("!!!!!!!!!!!!!!!!!!!!!!!!")
    add_dependencies(disk_image ${BIN_FILE}-target)
endif()

