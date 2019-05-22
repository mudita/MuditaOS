set(BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/port.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_tickless_gpt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_tickless_systick.c

        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051 CACHE INTERNAL "")


