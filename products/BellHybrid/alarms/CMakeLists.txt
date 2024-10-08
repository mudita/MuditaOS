add_library(alarms STATIC)
add_library(bell::alarms ALIAS alarms)

target_sources(alarms
    PRIVATE
        BellAlarmHandler.cpp
        src/actions/PlayAudioActions.cpp
        src/actions/NotifyGUIAction.cpp
        src/actions/NotifyGUIBedtimeReminderAction.cpp
        src/actions/FrontlightAction.cpp
        src/actions/IgnoreKeysAction.cpp

        include/AbstractAlarmAction.hpp
        include/BellAlarmHandler.hpp
        include/BellAlarmConstants.hpp
        src/actions/PlayAudioActions.hpp
        src/actions/FrontlightAction.hpp
        src/actions/NotifyGUIAction.hpp
        src/actions/IgnoreKeysAction.hpp
        src/actions/NotifyGUIBedtimeReminderAction.hpp
    PUBLIC
        include/popups/AlarmActivatedPopupRequestParams.hpp
        include/popups/AlarmDeactivatedPopupRequestParams.hpp
        include/popups/BedtimeReminderPopupRequestParams.hpp
        include/popups/ChargingNotificationPopupRequestParams.hpp
        include/popups/ChargingDoneNotificationPopupRequestParams.hpp
)

target_include_directories(alarms
    PRIVATE
        include

    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(alarms
   PRIVATE
        bell::db
        bell::appmgr
        bell::app-common
        bell::evtmgr
        bell::audio
        bell::paths
        apps-common
   PUBLIC
        module-db
        service-time
)
