add_library(application-bell-meditation-timer STATIC)
add_library(bell::app-meditation-timer ALIAS application-bell-meditation-timer)

target_include_directories(application-bell-meditation-timer
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            data
            include/application-bell-meditation-timer
            models
            presenter
            widgets
            windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-meditation-timer
    PRIVATE
        MeditationTimer.cpp
        models/ChimeInterval.cpp
        models/ChimeVolume.cpp
        models/StartDelay.cpp
        presenter/MeditationCountdownPresenter.cpp
        presenter/MeditationProgressPresenter.cpp
        presenter/MeditationTimerPresenter.cpp
        presenter/ReadyGoingPresenter.cpp
        presenter/SessionEndedPresenter.cpp
        presenter/SettingsPresenter.cpp
        presenter/StatisticsPresenter.cpp
        windows/MeditationMainWindow.cpp
        windows/MeditationCountdownWindow.cpp
        windows/MeditationRunningWindow.cpp
        windows/MeditationTimerWindow.cpp
        windows/SettingsWindow.cpp
        windows/StatisticsWindow.cpp
    PUBLIC
        include/application-bell-meditation-timer/MeditationTimer.hpp
)

target_link_libraries(application-bell-meditation-timer
    PRIVATE
        app
        bell::audio
        bell::app-common
        bell::app-main
        bell::alarms
        service-appmgr
        service-time
    PUBLIC
        apps-common
        module-gui
)
