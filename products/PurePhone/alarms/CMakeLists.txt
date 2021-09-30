add_library(alarms STATIC)
add_library(pure::alarms ALIAS alarms)

target_sources(alarms
    PRIVATE
        PureAlarmHandler.cpp
        src/actions/AlarmRecordAction.cpp
        src/actions/PlayAudioActions.cpp
        src/actions/NotifyGUIAction.cpp

        include/AbstractAlarmAction.hpp
        include/PureAlarmHandler.hpp
        src/actions/AlarmRecordAction.hpp
        src/actions/PlayAudioActions.hpp
        src/actions/NotifyGUIAction.hpp
    PUBLIC
)

target_include_directories(alarms
    PRIVATE
        include

    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(alarms
   PRIVATE
        module-vfs
        apps-common
   PUBLIC
        module-db
        service-time
)
