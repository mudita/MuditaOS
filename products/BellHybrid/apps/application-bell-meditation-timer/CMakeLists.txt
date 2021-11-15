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
        ApplicationBellMeditationTimer.cpp
        presenter/IntervalChimePresenter.cpp
        presenter/MeditationProgressPresenter.cpp
        presenter/MeditationTimerPresenter.cpp
        presenter/ReadyGoingPresenter.cpp
        presenter/SessionEndedPresenter.cpp 
        windows/IntervalChimeWindow.cpp
        windows/MeditationRunningWindow.cpp
        windows/MeditationTimerWindow.cpp
        windows/ReadyGoingWindow.cpp
        windows/SessionPausedWindow.cpp
    PUBLIC
        include/application-bell-meditation-timer/ApplicationBellMeditationTimer.hpp
)

target_link_libraries(application-bell-meditation-timer
    PRIVATE
        app
        bell::audio
        bell::app-common
        bell::app-main
        bellgui
        service-appmgr
        service-time
    PUBLIC
        apps-common
        module-gui
)
