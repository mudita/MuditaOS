add_library(application-bell-relaxation STATIC)
add_library(bell::app-relaxation ALIAS application-bell-relaxation)

target_include_directories(application-bell-relaxation
    PRIVATE
        $<BUILD_INTERFACE:
            include/application-bell-relaxation
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-relaxation
    PRIVATE
        ApplicationBellRelaxation.cpp
        presenter/RelaxationMainWindowPresenter.cpp
        presenter/RelaxationRunningProgressPresenter.cpp
        presenter/RelaxationRunningLoopPresenter.cpp
        presenter/RelaxationTimerSelectPresenter.cpp
        presenter/RelaxationVolumePresenter.cpp
        presenter/RelaxationPausedPresenter.cpp
        presenter/RelaxationEndedPresenter.cpp
        presenter/RelaxationLowBatteryPresenter.cpp
        widgets/RelaxationPlayer.cpp
        windows/RelaxationMainWindow.cpp
        windows/RelaxationPausedWindow.cpp
        windows/RelaxationRunningProgressWindow.cpp
        windows/RelaxationRunningLoopWindow.cpp
        windows/RelaxationTimerSelectWindow.cpp
        windows/RelaxationVolumeWindow.cpp
        windows/RelaxationEndedWindow.cpp
        windows/RelaxationLowBatteryWindow.cpp

        data/RelaxationCommon.hpp
        data/RelaxationStyle.hpp
        data/RelaxationAudioData.hpp
        data/RelaxationSwitchData.hpp
        widgets/RelaxationPlayer.hpp
        windows/RelaxationMainWindow.hpp
        presenter/RelaxationMainWindowPresenter.hpp
        presenter/RelaxationRunningProgressPresenter.hpp
        presenter/RelaxationRunningLoopPresenter.hpp
        presenter/RelaxationTimerSelectPresenter.hpp
        presenter/RelaxationVolumePresenter.hpp
        presenter/RelaxationPausedPresenter.hpp
        presenter/RelaxationEndedPresenter.hpp
        presenter/RelaxationLowBatteryPresenter.hpp
        windows/RelaxationMainWindow.hpp
        windows/RelaxationPausedWindow.hpp
        windows/RelaxationRunningProgressWindow.hpp
        windows/RelaxationRunningLoopWindow.hpp
        windows/RelaxationTimerSelectWindow.hpp
        windows/RelaxationVolumeWindow.hpp
        windows/RelaxationEndedWindow.hpp
        windows/RelaxationLowBatteryWindow.hpp

    PUBLIC
        include/application-bell-relaxation/ApplicationBellRelaxation.hpp
)

target_link_libraries(application-bell-relaxation
    PRIVATE
        apps-common
        bell::app-main
        bell::audio
        bell::alarms
        bell::paths
        Microsoft.GSL::GSL

    PUBLIC
        module-gui
        bell::app-common
)
