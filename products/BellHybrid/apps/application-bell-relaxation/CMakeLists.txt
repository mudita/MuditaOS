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
        presenter/RelaxationProgressPresenter.cpp
        presenter/RelaxationTimerSelectPresenter.cpp
        presenter/RelaxationVolumePresenter.cpp
        widgets/RelaxationPlayer.cpp
        windows/RelaxationMainWindow.cpp
        windows/RelaxationPausedWindow.cpp
        windows/RelaxationProgressWindow.cpp
        windows/RelaxationTimerSelectWindow.cpp
        windows/RelaxationVolumeWindow.cpp

        data/RelaxationCommon.hpp
        data/RelaxationStyle.hpp
        data/RelaxationAudioData.hpp
        widgets/RelaxationPlayer.hpp
        windows/RelaxationMainWindow.hpp
        presenter/RelaxationMainWindowPresenter.hpp
        presenter/RelaxationProgressPresenter.hpp
        presenter/RelaxationTimerSelectPresenter.hpp
        presenter/RelaxationVolumePresenter.hpp
        windows/RelaxationMainWindow.hpp
        windows/RelaxationPausedWindow.hpp
        windows/RelaxationProgressWindow.hpp
        windows/RelaxationTimerSelectWindow.hpp
        windows/RelaxationVolumeWindow.hpp

    PUBLIC
        include/application-bell-relaxation/ApplicationBellRelaxation.hpp
)

target_link_libraries(application-bell-relaxation
    PRIVATE
        apps-common
        bell::audio
        bell::alarms
        Microsoft.GSL::GSL

    PUBLIC
        module-gui
        bell::app-common
)
