add_library(alarms STATIC)
add_library(bell::alarms ALIAS alarms)

target_sources(alarms
    PRIVATE
        BellAlarmHandler.cpp
        src/AlarmSoundPaths.cpp
        src/actions/PlayAudioActions.cpp
        src/actions/NotifyGUIAction.cpp
        src/actions/FrontlightAction.cpp

        include/AbstractAlarmAction.hpp
        include/BellAlarmHandler.hpp
        src/actions/PlayAudioActions.hpp
        src/actions/FrontlightAction.hpp
        src/actions/NotifyGUIAction.hpp
    PUBLIC
        include/AlarmSoundPaths.hpp
        include/popups/AlarmActivatedPopupRequestParams.hpp
        include/popups/AlarmDeactivatedPopupRequestParams.hpp
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
        bell::db
        bell::app-common
        apps-common
   PUBLIC
        module-db
        service-time
)
