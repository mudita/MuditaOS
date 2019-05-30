set(BOARD_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/board.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/eink/ED028TC1.c
		
	CACHE INTERNAL "")

set(BOARD_DIR_INCLUDES  
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/fsl_drivers
	${CMAKE_CURRENT_SOURCE_DIR}/board/linux/eink 
	
	
	CACHE INTERNAL "")
