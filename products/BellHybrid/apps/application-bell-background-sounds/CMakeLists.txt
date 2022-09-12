add_library(application-bell-background-sounds STATIC)
add_library(bell::app-background-sounds ALIAS application-bell-background-sounds)

target_include_directories(application-bell-background-sounds
    PRIVATE
        $<BUILD_INTERFACE:
            include/application-bell-background-sounds
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-background-sounds
    PRIVATE
        ApplicationBellBackgroundSounds.cpp
        presenter/BGSoundsMainWindowPresenter.cpp
        presenter/BGSoundsProgressPresenter.cpp
        presenter/BGSoundsTimerSelectPresenter.cpp
        presenter/BGSoundsVolumePresenter.cpp
        widgets/BGSoundsPlayer.cpp
        widgets/SoundListItem.cpp
        windows/BGSoundsMainWindow.cpp
        windows/BGSoundsPausedWindow.cpp
        windows/BGSoundsProgressWindow.cpp
        windows/BGSoundsTimerSelectWindow.cpp
        windows/BGSoundsVolumeWindow.cpp

        data/BGSoundsCommon.hpp
        data/BGSoundsStyle.hpp
        data/BGSoundsAudioData.hpp
        widgets/BGSoundsPlayer.hpp
        windows/BGSoundsMainWindow.hpp
        presenter/BGSoundsMainWindowPresenter.hpp
        presenter/BGSoundsProgressPresenter.hpp
        presenter/BGSoundsTimerSelectPresenter.hpp
        presenter/BGSoundsVolumePresenter.hpp
        windows/BGSoundsMainWindow.hpp
        windows/BGSoundsPausedWindow.hpp
        windows/BGSoundsProgressWindow.hpp
        windows/BGSoundsTimerSelectWindow.hpp
        windows/BGSoundsVolumeWindow.hpp

    PUBLIC
        include/application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp
)

target_link_libraries(application-bell-background-sounds
    PRIVATE
        apps-common
        bell::audio
        bell::paths
        Microsoft.GSL::GSL

    PUBLIC
        module-gui
        bell::app-common
)
