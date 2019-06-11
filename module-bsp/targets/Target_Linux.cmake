
set(BOARD_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/board.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/eink/ED028TC1.c
	"${CMAKE_CURRENT_SOURCE_DIR}/board/linux/keyboard/linux_keyboard.cpp"
		
	CACHE INTERNAL "")

set(BOARD_DIR_INCLUDES  
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/fsl_drivers
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/eink 
	

	CACHE INTERNAL "")

set(BOARD_SOURCE_FILES_PROPERTIES ${BOARD_SOURCE_FILES_PROPERTIES} ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/board.cpp   PROPERTIES COMPILE_FLAGS -Wno-unused-function CACHE INTERNAL "")

