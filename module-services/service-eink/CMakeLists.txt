project(service-eink)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    ServiceEink.cpp
    EinkDisplay.cpp
    messages/ImageMessage.cpp
    messages/PrepareDisplayEarlyRequest.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_board_subdirectory(board)

target_link_libraries( ${PROJECT_NAME}
    module-utils
    service-gui
    Microsoft.GSL::GSL
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/messages"
)
