
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

include_directories(
    ${module-bsp_INCLUDE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/messages
    ${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}
)

target_sources( ${PROJECT_NAME}

	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/ServiceEink.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/messages/ImageMessage.cpp"
)

