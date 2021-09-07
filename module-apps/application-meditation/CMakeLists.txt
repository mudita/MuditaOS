# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-meditation STATIC)

target_include_directories(application-meditation
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            ${CMAKE_CURRENT_SOURCE_DIR}/include/application-meditation
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets
            ${CMAKE_CURRENT_SOURCE_DIR}/windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-meditation
    PRIVATE
        ApplicationMeditation.cpp
        data/MeditationTimerData.hpp
        data/Style.hpp
        widgets/IntervalBox.cpp
        widgets/IntervalBox.hpp
        widgets/MeditationTimer.cpp
        widgets/MeditationTimer.hpp
        widgets/TimerProperty.cpp
        widgets/TimerProperty.hpp
        windows/MeditationOptionsWindows.cpp
        windows/MeditationOptionsWindows.hpp
        windows/MeditationTimerWindow.cpp
        windows/MeditationTimerWindow.hpp
        windows/MeditationWindow.cpp
        windows/MeditationWindow.hpp
        windows/Names.hpp
    PUBLIC
        include/application-meditation/ApplicationMeditation.hpp
        include/application-meditation/OptionsData.hpp
)

target_link_libraries(application-meditation
    PRIVATE
        i18n
        module-sys
        module-utils
        module-vfs
        service-audio
        time
    PUBLIC
        apps-common
        module-gui
)
