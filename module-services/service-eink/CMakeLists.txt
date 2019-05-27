
if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    set( EINK_BOARD_PATH "board/rt1051" )
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(CMAKE_CXX_STANDARD 17)
    set( EINK_BOARD_PATH "board/linux" )
    add_subdirectory( "board/linux/renderer" )
else()
    message(FATAL_ERROR "Invalid target!")
endif()

message( "EINK BOARD PATH: ${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}" )

include_directories( ${CMAKE_PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}"
)

include_directories( ${PROJECT_NAME}

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}"
)

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}/ServiceEink.cpp"
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}/ServiceEink.hpp"
)

