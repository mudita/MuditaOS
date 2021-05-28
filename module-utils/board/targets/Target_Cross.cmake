set(BOARD_SOURCES ${BOARD_SOURCES}

        ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_rt1051.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cross/time_syscalls.cpp
        CACHE INTERNAL ""
        )

if (${LOG_REDIRECT} STREQUAL "RTT_JLINK")
    set(BOARD_SOURCES ${BOARD_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_writeRTT.cpp
            CACHE INTERNAL ""
            )
elseif (${LOG_REDIRECT} STREQUAL "RTT_LUART")
    set(BOARD_SOURCES ${BOARD_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_writeLUART.cpp
            CACHE INTERNAL ""
            )
elseif ((${LOG_REDIRECT} STREQUAL "RTT_SYSTEMVIEW") AND (${SYSTEM_VIEW_ENABLED} EQUAL 1))
    set(BOARD_SOURCES ${BOARD_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_writeSYSTEMVIEW.cpp
            CACHE INTERNAL ""
            )
endif ()

set(BOARD_DIR_INCLUDES CACHE INTERNAL "")


