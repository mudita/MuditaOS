function(add_image)
    cmake_parse_arguments(
        _ARG
        ""
        "PRODUCT;SYSROOT;IMAGE_PARTITIONS"
        "DEPENDS"
        ${ARGN}
    )

    set(SCRIPT_PATH ${CMAKE_SOURCE_DIR}/tools/generate_image.sh)

    set(DISK_IMAGE_NAME ${_ARG_PRODUCT}.img)
    set(DISK_IMAGE_PATH ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})
    if(DEFINED _ARG_IMAGE_PARTITIONS)
        set(IMAGE_PARTITIONS ${_ARG_IMAGE_PARTITIONS})
    else()
        set(IMAGE_PARTITIONS ${CMAKE_SOURCE_DIR}/config/products/${_ARG_PRODUCT}/image_partitions.map)
    endif()

    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS ${SCRIPT_PATH} ${_ARG_DEPENDS}
        COMMAND
            ${SCRIPT_PATH}
            ${DISK_IMAGE_NAME}
            ${IMAGE_PARTITIONS}
            ${CMAKE_BINARY_DIR}/${_ARG_SYSROOT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME} with: add_image: ${_ARG_PRODUCT} : ${_ARG_DEPENDS}"
    )

    message("Adding disk image target: ${DISK_IMAGE_NAME}")

    add_custom_target(${_ARG_PRODUCT}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})

endfunction()
