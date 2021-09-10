add_library(application-bell-meditation-timer STATIC)
add_library(bell::app-meditation-timer ALIAS application-bell-meditation-timer)

target_include_directories(application-bell-meditation-timer
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            data
            include/application-bell-meditation-timer
            widgets
            windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-meditation-timer
    PRIVATE
        ApplicationBellMeditationTimer.cpp
        windows/MeditationWindow.cpp
        windows/WithTimerWindow.cpp
        windows/IconTextWindow.cpp
        windows/MeditationTimerWindow.cpp
        windows/IntervalChimeWindow.cpp
        windows/ReadyGoingWindow.cpp
        windows/MeditationRunningWindow.cpp
        windows/SessionPausedWindow.cpp
        windows/SessionEndWindow.cpp
        widgets/UnityProgressBar.cpp
    PUBLIC
        include/application-bell-meditation-timer/ApplicationBellMeditationTimer.hpp
)

target_link_libraries(application-bell-meditation-timer
    PRIVATE
        bellgui
        service-appmgr
        service-time
    PUBLIC
        apps-common
        module-gui
)
