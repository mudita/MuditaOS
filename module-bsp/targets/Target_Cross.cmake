set(BOARD_SOURCES ${BOARD_SOURCES}

        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/eMMC/eMMC.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/keyboard/keyboard.cpp


        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross

        CACHE INTERNAL "")