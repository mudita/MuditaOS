include(Utils)

set(WITH_BOARD_REVISION CACHE STRING "Board revision" )

function(add_board_subdirectory)
    if(NOT ${ARGV0} STREQUAL "")
        set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/${ARGV0})
    else()
        set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    endif()

    if(NOT DEFINED BOARD)
        message(FATAL_ERROR "Board is not defined")
    endif()
    
    if (${BOARD} STREQUAL "bellpx" OR ${BOARD} STREQUAL "puretx")
        add_subdirectory_if_exists(${ROOT_DIR}/rt1051)
    elseif (${BOARD} STREQUAL "linux")
        add_subdirectory_if_exists(${ROOT_DIR}/${BOARD})
    else ()
        message(FATAL_ERROR "Board unsupported")
    endif ()

endfunction()

macro(select_board)
    if(NOT DEFINED PRODUCT)
        message(FATAL_ERROR "Cannot determine board without knowing the product")
    endif()

    set(BOARD_REVISION 1)

    if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
        set(BOARD linux)
    elseif(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        if(${PRODUCT} STREQUAL "PurePhone")
            set(BOARD puretx)
            set(BOARD_REVISION 7)
        elseif(${PRODUCT} STREQUAL "BellHybrid")
            set(BOARD bellpx)
        else()
            message(FATAL_ERROR "Unknown product: ${PRODUCT}")
        endif()
    else()
        message(FATAL_ERROR "Unsupported target: ${PROJECT_TARGET}")
    endif()

    # overwrite board revision with user selection
    if(NOT "${WITH_BOARD_REVISION}" STREQUAL "")
        set(BOARD_REVISION ${WITH_BOARD_REVISION})
    endif()

    if("${BOARD}" STREQUAL "")
        message(FATAL_ERROR "Cannot determine board selection.")
    endif()
endmacro()
