project(service-audio)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    ServiceAudio.cpp
    AudioServiceAPI.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    service-appmgr
    service-bluetooth
    service-db
    module-bluetooth
    module-audio
    module-utils
    module-services
)


target_include_directories(${PROJECT_NAME}
    PUBLIC
       ${CMAKE_CURRENT_LIST_DIR}
)

