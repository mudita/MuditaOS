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
        presenter/FrontlightPresenter.cpp
        presenter/PrewakeUpPresenter.cpp
        presenter/TimeUnitsPresenter.cpp
        widgets/TemperatureUnitListItem.cpp
        widgets/TimeFormatSetListItem.cpp
        widgets/TimeSetListItem.cpp
        windows/BellSettingsAdvancedWindow.cpp
        windows/BellSettingsAlarmSettingsWindow.cpp
        windows/BellSettingsBedtimeToneWindow.cpp
        windows/BellSettingsFinishedWindow.cpp
        windows/BellSettingsFrontlight.cpp
        windows/BellSettingsHomeViewWindow.cpp
        windows/BellSettingsPrewakeUpWindow.cpp
        windows/BellSettingsTimeUnitsWindow.cpp
        windows/BellSettingsTurnOffWindow.cpp
        windows/BellSettingsWindow.cpp

        models/FrontlightModel.hpp
        models/PrewakeUpModel.hpp
        models/TemperatureUnitModel.hpp
        models/TimeUnitsModel.hpp
        presenter/FrontlightPresenter.hpp
        presenter/PrewakeUpPresenter.hpp
        presenter/TimeUnitsPresenter.hpp
        widgets/TemperatureUnitListItem.hpp
        widgets/TimeFormatSetListItem.hpp
        widgets/TimeSetListItem.hpp
        windows/BellSettingsAdvancedWindow.hpp
        windows/BellSettingsAlarmSettingsWindow.hpp
        windows/BellSettingsBedtimeToneWindow.hpp
        windows/BellSettingsFrontlight.hpp
        windows/BellSettingsHomeViewWindow.hpp
        windows/BellSettingsPrewakeUpWindow.hpp
        windows/BellSettingsTimeUnitsWindow.hpp
        windows/BellSettingsTurnOffWindow.hpp
        windows/BellSettingsWindow.hpp

    PRIVATE
        windows/BellSettingsFinishedWindow.hpp
        data/FinishedWindowMessageData.hpp

    PUBLIC
        include/application-bell-settings/ApplicationBellSettings.hpp
)

target_link_libraries(application-bell-settings
    PRIVATE
        bellgui
        bell::app-common
        bell::db
        service-appmgr
    PUBLIC
        module-gui
)
