project(service-audio)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    ServiceAudio.cpp
    AudioServiceAPI.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-apps
        module-audio
        module-bluetooth
        module-db
        module-services
        module-utils
        service-appmgr
        service-bluetooth
        service-cellular
        service-db
        service-desktop
)


target_include_directories(${PROJECT_NAME}
    PUBLIC
       ${CMAKE_CURRENT_LIST_DIR}
)

