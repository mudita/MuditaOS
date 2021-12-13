function(add_image)
    cmake_parse_arguments(
        _ARG
        ""
        "PRODUCT;SYSROOT;IMAGE_PARTITIONS;LUTS"
        "DEPENDS"
        ${ARGN}
    )

    if(NOT ${PROJECT_TARGET_NAME} STREQUAL "linux")
        set(HAS_BOOTFILE YES)
        set(HAS_UPDATER YES)
        set(HAS_VERSION YES)
    endif()

    set(SCRIPT_PATH ${CMAKE_SOURCE_DIR}/tools/generate_image.sh)

    set(DISK_IMAGE_NAME ${_ARG_PRODUCT}.img)
    set(DISK_IMAGE_PATH ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})
    if(DEFINED _ARG_IMAGE_PARTITIONS)
        set(IMAGE_PARTITIONS ${_ARG_IMAGE_PARTITIONS})
    else()
        set(IMAGE_PARTITIONS ${CMAKE_SOURCE_DIR}/config/products/${_ARG_PRODUCT}/image_partitions.map)
    endif()

    if(HAS_BOOTFILE)
        set(BIN_FILE_TARGET ${_ARG_PRODUCT}-boot.bin)
        set(BIN_FILE_PATH ${CMAKE_BINARY_DIR}/sysroot/sys/current/${_ARG_PRODUCT}-boot.bin)
    else()
        set(BIN_FILE_PATH "")
    endif()


    if(HAS_UPDATER)
        set(UPDATER_FILE_PATH ${CMAKE_BINARY_DIR}/updater.bin)
    else()
        set(UPDATER_FILE_PATH "")
    endif()

    # set(COMMAND_DEPENDS "genlittlefs")

    if(HAS_VERSION)
        set(VERSION_FILE_PATH ${CMAKE_BINARY_DIR}/${_ARG_PRODUCT}-version.json)
    else()
        set(VERSION_FILE_PATH "")
    endif()

    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS ${SCRIPT_PATH} ${_ARG_DEPENDS}
        COMMAND
            ${SCRIPT_PATH}
            ${DISK_IMAGE_NAME}
            ${IMAGE_PARTITIONS}
            ${CMAKE_BINARY_DIR}/${_ARG_SYSROOT}
            "${_ARG_LUTS}"
            "${VERSION_FILE_PATH}"
            "${BIN_FILE_PATH}"
            "${UPDATER_FILE_PATH}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME} with: add_image: ${_ARG_PRODUCT} : ${_ARG_DEPENDS}"
    )

    message("Adding disk image target: ${DISK_IMAGE_NAME}")

    add_custom_target(${_ARG_PRODUCT}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})

endfunction()

