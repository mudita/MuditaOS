add_library(service-alarm STATIC)

target_include_directories(service-alarm
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/service-alarm>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(service-alarm
    PRIVATE
        $<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:board/linux/ServiceAlarmImp.cpp>
        $<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:board/rt1051/ServiceAlarmImp.cpp>
        AlarmObject.cpp
        ServiceAlarm.cpp
        ServiceAlarmAPI.cpp
    PUBLIC
        include/service-alarm/AlarmObject.hpp
        include/service-alarm/ServiceAlarm.hpp
        include/service-alarm/ServiceAlarmAPI.hpp
        include/service-alarm/ServiceAlarmImp.hpp
        include/service-alarm/ServiceAlarmMessages.hpp
)

target_link_libraries(service-alarm
    PRIVATE
        log
        messagetype
        module-sys
        time
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
