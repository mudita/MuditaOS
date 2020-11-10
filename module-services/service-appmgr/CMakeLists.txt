project(service-appmgr)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    ApplicationManifest.cpp
    Controller.cpp
    model/ApplicationManager.cpp
    model/ApplicationHandle.cpp
    model/ApplicationsRegistry.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-cellular
        module-apps
        module-audio
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

