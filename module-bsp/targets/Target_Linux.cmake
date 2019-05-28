set(BOARD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/board.cpp 

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/keyboard/linux_keyboard.cpp"
 CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

            ${CMAKE_CURRENT_SOURCE_DIR}/board/linux
            ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/keyboard



        CACHE INTERNAL "")
