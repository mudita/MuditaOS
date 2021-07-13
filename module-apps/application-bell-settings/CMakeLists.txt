# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-bell-settings STATIC)

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
        presenter/TimeUnitsPresenter.cpp
        widgets/TimeSetSpinnerListItem.cpp
        windows/BellSettingsWindow.cpp
        windows/BellSettingsAdvancedWindow.cpp
        windows/BellSettingsTimeUnitsWindow.cpp

        models/TimeUnitsModel.hpp
        presenter/TimeUnitsPresenter.hpp
        widgets/TimeSetSpinnerListItem.hpp
        windows/BellSettingsWindow.hpp
        windows/BellSettingsAdvancedWindow.hpp
        windows/BellSettingsTimeUnitsWindow.hpp

    PUBLIC
        include/application-bell-settings/ApplicationBellSettings.hpp
)

target_link_libraries(application-bell-settings
    PRIVATE
        bellgui
        service-appmgr
    PUBLIC
        apps-common
        module-gui
)
