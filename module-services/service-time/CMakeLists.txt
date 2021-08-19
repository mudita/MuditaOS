project(service-time)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}" )

set(SOURCES
    AlarmHandlerFactory.cpp
    ServiceTime.cpp
    TimeManager.cpp
    RTCcommand.cpp
    TimezoneHandler.cpp
    internal/StaticData.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        time
        service-time-api
        module-sys
        module-apps
)

add_subdirectory(api)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
