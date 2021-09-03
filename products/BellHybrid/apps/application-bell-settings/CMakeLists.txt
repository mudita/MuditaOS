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
        models/TimeUnitsModel.cpp
        models/TemperatureUnitModel.cpp
        models/FrontlightModel.cpp
        presenter/TimeUnitsPresenter.cpp
        presenter/FrontlightPresenter.cpp
        widgets/TimeSetListItem.cpp
        widgets/TimeFormatSetListItem.cpp
        widgets/TemperatureUnitListItem.cpp
        windows/BellSettingsWindow.cpp
        windows/BellSettingsAdvancedWindow.cpp
        windows/BellSettingsTimeUnitsWindow.cpp
        windows/BellSettingsFinishedWindow.cpp
        windows/BellSettingsAlarmSettingsWindow.cpp
        windows/BellSettingsBedtimeToneWindow.cpp
        windows/BellSettingsFrontlight.cpp
        windows/BellSettingsHomeViewWindow.cpp
        windows/BellSettingsTurnOffWindow.cpp

        models/TimeUnitsModel.hpp
        models/TemperatureUnitModel.hpp
        models/FrontlightModel.hpp
        presenter/TimeUnitsPresenter.hpp
        presenter/FrontlightPresenter.hpp
        widgets/TimeSetListItem.hpp
        widgets/TimeFormatSetListItem.hpp
        widgets/TemperatureUnitListItem.hpp
        windows/BellSettingsWindow.hpp
        windows/BellSettingsAdvancedWindow.hpp
        windows/BellSettingsAlarmSettingsWindow.hpp
        windows/BellSettingsBedtimeToneWindow.hpp
        windows/BellSettingsFrontlight.hpp
        windows/BellSettingsHomeViewWindow.hpp
        windows/BellSettingsTimeUnitsWindow.hpp
        windows/BellSettingsTurnOffWindow.hpp

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
