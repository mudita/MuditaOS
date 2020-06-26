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
        ServiceFota.cpp
        api/FotaServiceAPI.cpp
    PUBLIC
        messages/FotaMessages.hpp
)

