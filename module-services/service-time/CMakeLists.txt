project(service-time)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}" )

set(SOURCES
    ServiceTime.cpp
    timeEvents/TimeEvents.cpp
    timeEvents/CalendarTimeEvents.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-apps
        module-sys
        service-appmgr
        service-cellular
        service-desktop
        service-gui
)
