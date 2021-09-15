macro(lwext4_output_configure)
    get_property(
        definitions
        TARGET lwext4::lwext4
        PROPERTY COMPILE_DEFINITIONS
    )
    file(WRITE
         ${CMAKE_CURRENT_BINARY_DIR}/include/generated/ext4_config.h
         "")
    foreach(item ${definitions})
        string(REGEX MATCH "^CONFIG_" match_res ${item})
        if(match_res)
            string(REGEX REPLACE "=(.+)$" "" replace_res ${item})
            string(CONFIGURE
                   "#define ${replace_res} ${CMAKE_MATCH_1}"
                   output_str)
            file(APPEND
                 ${CMAKE_CURRENT_BINARY_DIR}/include/generated/ext4_config.h
                 "${output_str}\n")
        endif()
    endforeach()
endmacro()
lwext4_output_configure()