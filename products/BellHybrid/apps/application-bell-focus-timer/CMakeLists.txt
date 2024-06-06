# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-bell-focus-timer STATIC)
add_library(bell::app-focus-timer ALIAS application-bell-focus-timer)

target_include_directories(application-bell-focus-timer
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            include/application-bell-focus-timer
            data
            models
            presenter
            windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-focus-timer
    PRIVATE
        ApplicationFocusTimer.cpp
        
        windows/FocusMainWindow.cpp
        windows/FocusSettingsWindow.cpp
        windows/FocusTimerWindow.cpp

        presenter/FocusMainPresenter.cpp
        presenter/FocusSettingsPresenter.cpp
        presenter/FocusTimerPresenter.cpp

        models/FocusSettingsModel.cpp
        models/NotificationVolumeModel.cpp

    PUBLIC
        include/application-bell-focus-timer/ApplicationFocusTimer.hpp
)

target_link_libraries(application-bell-focus-timer
    PRIVATE
        app
        bell::app-common
        bell::app-main
        bell::audio
        bell::keymap
        bell::paths
        bell::db

    PUBLIC
        module-gui
)
