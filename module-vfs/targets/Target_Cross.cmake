
set( BOARD_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/board/cross/free_rtos_custom/portable/ff_eMMC_user_disk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/board/cross/free_rtos_custom/portable/common.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/board/cross/free_rtos_custom/portable/vfs.cpp
	CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES  ${CMAKE_CURRENT_SOURCE_DIR}/board/cross/free_rtos_custom/include CACHE INTERNAL "")
