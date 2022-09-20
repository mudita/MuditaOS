# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-bell-settings STATIC)
add_library(bell::app-settings ALIAS application-bell-settings)

target_include_directories(application-bell-settings
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            data
            include/application-bell-settings
            models
            presenter
            widgets
            windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-settings
    PRIVATE
        ApplicationBellSettings.cpp
        models/TemperatureUnitModel.cpp
        models/DateTimeUnitsModel.cpp
        models/AboutYourBellModel.cpp
        models/FrontlightListItemProvider.cpp
        models/FrontlightModel.cpp
        models/alarm_settings/AlarmSettingsListItemProvider.cpp
        models/alarm_settings/AlarmSettingsModel.cpp
        models/alarm_settings/BedtimeSettingsListItemProvider.cpp
        models/alarm_settings/PrewakeUpListItemProvider.cpp
        models/alarm_settings/PrewakeUpSettingsModel.cpp
        models/alarm_settings/SnoozeListItemProvider.cpp
        models/alarm_settings/SnoozeSettingsModel.cpp
        models/alarm_settings/SettingsListItemProvider.cpp
        models/LayoutModel.cpp

        presenter/BedtimeSettingsPresenter.cpp
        presenter/TimeUnitsPresenter.cpp
        presenter/FrontlightPresenter.cpp
        presenter/AboutYourBellWindowPresenter.cpp
        presenter/LanguageWindowPresenter.cpp
        presenter/LayoutWindowPresenter.cpp
        presenter/alarm_settings/AlarmSettingsPresenter.cpp
        presenter/alarm_settings/PrewakeUpPresenter.cpp
        presenter/alarm_settings/SnoozePresenter.cpp

        widgets/TemperatureUnitListItem.cpp
        widgets/TimeFormatSetListItem.cpp
        widgets/DateSetListItem.cpp
        widgets/TimeSetListItem.cpp
        widgets/AboutYourBellListItem.cpp
        widgets/DialogYesNo.cpp

        windows/BellSettingsBedtimeToneWindow.cpp
        windows/BellSettingsTimeUnitsWindow.cpp
        windows/BellSettingsHomeViewWindow.cpp
        windows/BellSettingsWindow.cpp
        windows/AboutYourBellWindow.cpp
        windows/BellSettingsLanguageWindow.cpp
        windows/BellSettingsLayoutWindow.cpp
        windows/BellSettingsFrontlightWindow.cpp
        windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.cpp
        windows/alarm_settings/BellSettingsAlarmSettingsMenuWindow.cpp
        windows/alarm_settings/BellSettingsAlarmSettingsWindow.cpp
        windows/alarm_settings/BellSettingsPrewakeUpWindow.cpp

        models/AboutYourBellModel.hpp
        models/FrontlightListItemProvider.hpp
        models/alarm_settings/AbstractPrewakeUpSettingsModel.hpp
        models/alarm_settings/AbstractSnoozeSettingsModel.hpp
        models/alarm_settings/AlarmSettingsListItemProvider.hpp
        models/alarm_settings/BedtimeSettingsListItemProvider.cpp
        models/alarm_settings/PrewakeUpListItemProvider.hpp
        models/alarm_settings/PrewakeUpSettingsModel.hpp
        models/alarm_settings/SnoozeListItemProvider.hpp
        models/alarm_settings/SnoozeSettingsModel.hpp
        models/alarm_settings/SettingsListItemProvider.hpp

        presenter/BedtimeSettingsPresenter.hpp
        presenter/FrontlightPresenter.hpp
        presenter/AboutYourBellWindowPresenter.hpp
        presenter/LanguageWindowPresenter.hpp
        presenter/LayoutWindowPresenter.hpp
        presenter/alarm_settings/AlarmSettingsPresenter.hpp
        presenter/alarm_settings/PrewakeUpPresenter.hpp
        presenter/alarm_settings/SnoozePresenter.hpp

        widgets/TemperatureUnitListItem.hpp
        widgets/TimeFormatSetListItem.hpp
        widgets/TimeSetListItem.hpp
        widgets/AboutYourBellListItem.hpp
        widgets/DialogYesNo.hpp

        windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp
        windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp
        windows/BellSettingsBedtimeToneWindow.hpp
        windows/BellSettingsHomeViewWindow.hpp
        windows/BellSettingsWindow.hpp
        windows/AboutYourBellWindow.hpp
        windows/BellSettingsLanguageWindow.hpp
        windows/BellSettingsLayoutWindow.hpp
        windows/BellSettingsFrontlightWindow.hpp
        windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp
        windows/alarm_settings/BellSettingsAlarmSettingsMenuWindow.hpp
        windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp
        windows/alarm_settings/BellSettingsPrewakeUpWindow.hpp

        data/FinishedWindowMessageData.hpp

    PUBLIC
        include/application-bell-settings/models/DateTimeUnitsModel.hpp
        include/application-bell-settings/presenter/TimeUnitsPresenter.hpp
        include/application-bell-settings/windows/BellSettingsTimeUnitsWindow.hpp
        include/application-bell-settings/ApplicationBellSettings.hpp
)

target_link_libraries(application-bell-settings
    PRIVATE
        app
        bell::audio
        bell::db
        bell::paths
        bell::app-main
        bell::appmgr
        service-appmgr
        apps-common
    PUBLIC
        module-gui
        bell::app-common
)
