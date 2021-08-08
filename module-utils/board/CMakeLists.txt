add_board_subdirectory()
target_link_libraries(board PUBLIC module-bsp PRIVATE segger::rtt)
