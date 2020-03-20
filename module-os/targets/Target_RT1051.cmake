set(BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_tickless_gpt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_tickless_systick.c

        CACHE INTERNAL ""
        )

#Substitute FreeRTOS SystemvView sources if enabled
if(${SYSTEM_VIEW_ENABLED})
    set(SOURCES ${SOURCES}

            ${CMAKE_CURRENT_SOURCE_DIR}/systemviewPatch/port.c
            )
else()
    set(SOURCES ${SOURCES}

            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/port.c
            )
endif()

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051
        CACHE INTERNAL "")

#Add FreeRTOS SystemvView headers if enabled
if(${SYSTEM_VIEW_ENABLED})
    set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

            ${CMAKE_CURRENT_SOURCE_DIR}/systemviewPatch
            CACHE INTERNAL "")
endif()

