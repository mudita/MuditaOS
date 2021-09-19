add_library(board STATIC)

target_link_libraries(board PRIVATE log-api)

target_include_directories(board
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>    
)

target_sources(board
    PUBLIC
        include/Board.hpp
)

add_board_subdirectory()
