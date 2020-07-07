set(BOARD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/port.c CACHE INTERNAL "")

set(BOARD_DIR_INCLUDES  ${CMAKE_CURRENT_SOURCE_DIR}/board/linux CACHE INTERNAL "")

#Suppress unused variable warning
set_source_files_properties(${BOARD_SOURCES}
        PROPERTIES COMPILE_FLAGS
	"-Wno-unused-but-set-variable"
)
