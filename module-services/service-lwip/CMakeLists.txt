project(service-lwip)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    ServiceLwIP.cpp
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    lwip::lwip
)
