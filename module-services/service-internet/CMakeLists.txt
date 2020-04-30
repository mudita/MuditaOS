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
        g711.c
        ServiceInternet.cpp
    PUBLIC
        api/InternetServiceAPI.cpp
        g711.h
        messages/InternetMessage.hpp
)

