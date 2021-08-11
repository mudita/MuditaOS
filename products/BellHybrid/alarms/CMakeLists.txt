add_library(alarms STATIC)

target_sources(alarms
    PRIVATE
        BellAlarmHandler.cpp
        AlarmHandlerActions.cpp
)

target_include_directories(alarms
    PRIVATE
        include
        
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(alarms
   PUBLIC
        module-db
        service-alarm
)
