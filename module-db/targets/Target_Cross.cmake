set(BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/sqlite3vfs.cpp

        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/board/cross CACHE INTERNAL "")



set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/board/cross/sqlite3vfs.cpp PROPERTIES COMPILE_FLAGS -Wno-overflow)