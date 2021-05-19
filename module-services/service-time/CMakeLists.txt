project(service-time)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}" )

set(SOURCES
    ServiceTime.cpp
    TimeManager.cpp
    RTCcommand.cpp
    TimezoneHandler.cpp
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
        module-sys
        module-apps
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
