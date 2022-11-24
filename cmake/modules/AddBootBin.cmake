function(add_boot_bin SOURCE_TARGET)
    set(BIN_FILE ${SYSROOT_PATH}/system_a/bin/boot.bin)

    set_target_properties(
            ${SOURCE_TARGET}
        PROPERTIES
            BIN_FILE ${BIN_FILE}
    )

    if (ENABLE_SECURE_BOOT)
        IF(EXISTS ${SIGN_CLIENT_PATH}/signclient.py)
            add_custom_command(
                COMMENT "Generate signed boot.bin (Secure Boot)"
                OUTPUT ${BIN_FILE}
                DEPENDS ${SOURCE_TARGET}
                COMMAND python3 ${SIGN_CLIENT_PATH}/signclient.py --in_file $<TARGET_FILE:${SOURCE_TARGET}> --out_file=${BIN_FILE} --keystore ${KEYSTORE} --keyslot ${KEYSLOT} --server ${SERVER} --login ${LOGIN}
                VERBATIM
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                )
        else()
            message( FATAL_ERROR "signclient.py not found in ${SIGN_CLIENT_PATH}" )
        endif()
    else ()
        add_custom_command(
            COMMENT "Generate ${SOURCE_TARGET}-boot.bin"
            OUTPUT ${BIN_FILE}
            DEPENDS ${SOURCE_TARGET}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${SOURCE_TARGET}> ${BIN_FILE}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
    endif()
    message("Adding '${SOURCE_TARGET}-boot.bin' target")
    add_custom_target(${SOURCE_TARGET}-boot.bin
        DEPENDS ${BIN_FILE})
endfunction()
