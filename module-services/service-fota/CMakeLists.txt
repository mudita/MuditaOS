project(service-fota)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}" )

set(SOURCES
    ServiceFota.cpp
    FotaUrcHandler.cpp
    FotaServiceAPI.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
       module-cellular
       module-db
       module-sys
       service-cellular
       service-desktop
)

