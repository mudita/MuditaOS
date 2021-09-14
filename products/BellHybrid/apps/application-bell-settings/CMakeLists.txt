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
        models/FrontlightModel.cpp
        models/PrewakeUpModel.cpp
        models/TemperatureUnitModel.cpp
        models/TimeUnitsModel.cpp
        models/alarm_settings/SnoozeListItemProvider.cpp

        presenter/FrontlightPresenter.cpp
        presenter/PrewakeUpPresenter.cpp
        presenter/TimeUnitsPresenter.cpp
        presenter/alarm_settings/SnoozePresenter.cpp

        widgets/TimeSetListItem.cpp
        widgets/TimeFormatSetListItem.cpp
        widgets/TemperatureUnitListItem.cpp

        windows/BellSettingsWindow.cpp
        windows/BellSettingsBedtimeToneWindow.cpp
        windows/BellSettingsFrontlight.cpp
        windows/BellSettingsHomeViewWindow.cpp
        windows/BellSettingsPrewakeUpWindow.cpp
        windows/BellSettingsTurnOffWindow.cpp
        windows/advanced/BellSettingsAdvancedWindow.cpp
        windows/advanced/BellSettingsTimeUnitsWindow.cpp
        windows/alarm_settings/BellSettingsAlarmSettingsWindow.cpp
        windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.cpp

        models/PrewakeUpModel.hpp
        models/TimeUnitsModel.hpp
        models/alarm_settings/SnoozeListItemProvider.hpp

        presenter/FrontlightPresenter.hpp
        presenter/PrewakeUpPresenter.hpp
        presenter/TimeUnitsPresenter.hpp
        presenter/alarm_settings/SnoozePresenter.hpp

        widgets/TimeSetListItem.hpp

        windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp
        windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp
        windows/advanced/BellSettingsTimeUnitsWindow.hpp
        windows/advanced/BellSettingsAdvancedWindow.hpp
        windows/BellSettingsBedtimeToneWindow.hpp
        windows/BellSettingsFrontlight.hpp
        windows/BellSettingsHomeViewWindow.hpp
        windows/BellSettingsPrewakeUpWindow.hpp
        windows/BellSettingsTurnOffWindow.hpp
        windows/BellSettingsWindow.hpp

        data/FinishedWindowMessageData.hpp

    PUBLIC
        include/application-bell-settings/ApplicationBellSettings.hpp
)

target_link_libraries(application-bell-settings
    PRIVATE
        app
        bellgui
        bell::app-common
        bell::db
        service-appmgr
        apps-common
    PUBLIC
        module-gui
        bell::app-common
)
