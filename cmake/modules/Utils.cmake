macro(print_var VARIABLE)
    if(${VARIABLE})
        message(STATUS "${Green}${VARIABLE}: '${Orange}${${VARIABLE}}${ColourReset}'")
    else()
        message(STATUS "${Orange}No such variable: '${Red}${VARIABLE}${ColourReset}'")
    endif()
endmacro()

function(strip_executable TARGET)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(STRIP_PARAMS "-S")
    else()
        set(STRIP_PARAMS --strip-debug --strip-unneeded)
    endif ()

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_STRIP} ${STRIP_PARAMS} $<TARGET_FILE:${TARGET}>
            COMMENT "Striping  ${TARGET}"
            )
    endif()

    if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
                    $<TARGET_FILE:${TARGET}>
                    $<TARGET_FILE:${TARGET}>.debug
            COMMAND ${CMAKE_STRIP} ${STRIP_PARAMS} $<TARGET_FILE:${TARGET}>
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:${TARGET}>.debug
                    $<TARGET_FILE:${TARGET}>
            COMMENT "Striping  ${TARGET}"
            )
    endif()
endfunction()

function(add_subdirectory_if_exists)
    set(_DIR ${ARGV0})
    if(IS_DIRECTORY ${_DIR})
        add_subdirectory(${_DIR})
    endif()
endfunction()
