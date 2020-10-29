project(service-eink)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )
if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    set( EINK_BOARD_PATH "board/rt1051" )
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set( EINK_BOARD_PATH "board/linux" )
    add_subdirectory( "board/linux/renderer" )
else()
    message(FATAL_ERROR "Invalid target!")
endif()

message( "EINK BOARD PATH: ${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}" )

add_library(${PROJECT_NAME} STATIC)

target_link_libraries( ${PROJECT_NAME}
    module-utils
    service-gui
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/messages"
        "${CMAKE_CURRENT_LIST_DIR}/${EINK_BOARD_PATH}"
)

target_sources( ${PROJECT_NAME}
    PRIVATE
        ServiceEink.cpp
        messages/ImageMessage.cpp
)

