project(service-antenna)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    ServiceAntenna.cpp
    api/AntennaServiceAPI.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-apps
        module-cellular
        module-utils
        service-cellular
        eventstore
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)


