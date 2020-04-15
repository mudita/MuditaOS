include_directories( ${CMAKE_PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories( ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}" )

target_sources( ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/g711.c
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/api/InternetServiceAPI.cpp
        ${CMAKE_CURRENT_LIST_DIR}/g711.h
        ${CMAKE_CURRENT_LIST_DIR}/messages/InternetMessage.hpp
)

