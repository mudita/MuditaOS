add_library(board STATIC)

target_link_libraries(board PRIVATE log-api)

add_board_subdirectory()
