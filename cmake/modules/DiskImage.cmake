function(add_image)
    cmake_parse_arguments(
        _ARG
        ""
        "PRODUCT;SYSROOT;ASSETS"
        ""
        ${ARGN}
    )

    if(NOT ${PROJECT_TARGET_NAME} STREQUAL "linux")
        set(HAS_BOOTFILE YES)
        set(RUNS_ON_HARDWARE YES)
    endif()

    set(SCRIPT_PATH ${CMAKE_SOURCE_DIR}/generate_image.sh)

    set(DISK_IMAGE_NAME ${_ARG_PRODUCT}.img)
    set(DISK_IMAGE_PATH ${CMAKE_BINARY_DIR}/${DISK_IMAGE_NAME})

    if(HAS_BOOTFILE)
        set(BIN_FILE_TARGET ${_ARG_PRODUCT}-boot.bin)
        set(BIN_FILE_PATH ${CMAKE_BINARY_DIR}/sysroot/sys/current/${_ARG_PRODUCT}-boot.bin)
    else()
        set(BIN_FILE_PATH "")
    endif()

    set(COMMAND_DEPENDS "genlittlefs")
    list(APPEND COMMNDS_DEPENDS ${SCRIPT_PATH})
    if(_ARG_ASSETS)
        list(APPEND COMMAND_DEPENDS ${_ARG_ASSETS})
    endif()
    if(RUNS_ON_HARDWARE)
        list(APPEND COMMAND_DEPENDS pureflash)
    endif()
    if(HAS_BOOTFILE)
        list(APPEND COMMAND_DEPENDS ${BIN_FILE_TARGET})
    endif()

    add_custom_command(
        OUTPUT ${DISK_IMAGE_NAME}
        DEPENDS ${COMMAND_DEPENDS}
        COMMAND
            ${SCRIPT_PATH}
            ${DISK_IMAGE_NAME}
            ${CMAKE_BINARY_DIR}/${_ARG_SYSROOT}
            "${BIN_FILE_PATH}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate ${DISK_IMAGE_NAME}"
    )

    message("Adding disk image target: ${DISK_IMAGE_NAME}")

    add_custom_target(${_ARG_PRODUCT}-disk-img
        DEPENDS ${DISK_IMAGE_NAME})

endfunction()

