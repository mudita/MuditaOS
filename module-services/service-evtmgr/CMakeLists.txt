project(service-evtmgr)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
        EventManager.cpp
        WorkerEvent.cpp
        alarm/EventManagerAlarm.cpp
        api/EventManagerServiceAPI.cpp
        messages/Message.cpp
        LightControl.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-audio
        service-appmgr
        service-cellular
        service-desktop
        module-apps
        module-utils
        module-gui

)

