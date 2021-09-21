add_library(board-config INTERFACE)
add_board_subdirectory()
target_link_libraries(board INTERFACE board-config)
