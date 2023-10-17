function(add_hex_target SOURCE_TARGET)
    set(HEX_FILE ${SOURCE_TARGET}.hex)
    add_custom_command(
        COMMENT "Generate ${HEX_FILE}"
        OUTPUT ${CMAKE_BINARY_DIR}/${HEX_FILE}
        DEPENDS ${SOURCE_TARGET}
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${SOURCE_TARGET}> ${CMAKE_BINARY_DIR}/${HEX_FILE}

        )
    add_custom_target(
        ${HEX_FILE}-target ALL
        DEPENDS ${CMAKE_BINARY_DIR}/${HEX_FILE}
        )
endfunction()
