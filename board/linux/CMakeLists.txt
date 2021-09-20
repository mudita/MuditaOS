set_target_properties(board PROPERTIES LINKER_LANGUAGE CXX)
add_subdirectory(libiosyscalls)

target_include_directories(board
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_sources(board
    PUBLIC
        board/board.h
        board/macros.h
)
