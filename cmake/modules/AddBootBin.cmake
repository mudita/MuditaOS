function(add_boot_bin SOURCE_TARGET)
    set(BIN_FILE ${CMAKE_BINARY_DIR}/sysroot/sys/current/${SOURCE_TARGET}-boot.bin)

    set_target_properties(
            ${SOURCE_TARGET}
        PROPERTIES
            BIN_FILE ${BIN_FILE}
    )

    if (ENABLE_SECURE_BOOT)
        set (SREC_FILE ${CMAKE_PROJECT_NAME}.srec)
        # .srec file required by elftosb
        add_custom_command(
            COMMENT "Generate ${CMAKE_PROJECT_NAME}.srec"
            OUTPUT ${CMAKE_BINARY_DIR}/${SREC_FILE}
            DEPENDS ${SOURCE_TARGET}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${SOURCE_TARGET}> ${SREC_FILE}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )

        add_custom_command(
            COMMENT "Generate signed boot.bin (Secure Boot)"
            OUTPUT ${BIN_FILE}
            DEPENDS ${CMAKE_BINARY_DIR}/${SREC_FILE}
            COMMAND ${CMAKE_SOURCE_DIR}/config/elftosb_wrapper.sh "${ELFTOSB_PATH}" "${CST_PATH}" -f imx -V
            -c ${CMAKE_BINARY_DIR}/imx_authenticated_hab.bd
            -o ${BIN_FILE}
            ${CMAKE_BINARY_DIR}/${SREC_FILE}
            VERBATIM
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
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
