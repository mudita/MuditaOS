project(service-eink)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    ServiceEink.cpp
    api/ServiceEinkApi.cpp
    internal/StaticData.cpp
    messages/ImageMessage.cpp
    messages/PrepareDisplayEarlyRequest.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_board_subdirectory(board)

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        module-bsp
        messagetype
        module-sys
    PRIVATE
        Microsoft.GSL::GSL
        module-utils
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/messages"
)
