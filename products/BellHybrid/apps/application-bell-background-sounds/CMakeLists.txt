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
        data/BGSoundsVolumeListItem.cpp
        models/BGSoundsVolumeModel.cpp
        presenter/BGSoundsVolumePresenter.cpp
        windows/BGSoundsMainWindow.cpp
        windows/BGSoundsVolumeWindow.cpp

        data/BGSoundsVolumeListItem.hpp
        models/BGSoundsVolumeModel.hpp
        presenter/BGSoundsVolumePresenter.hpp
        windows/BGSoundsMainWindow.hpp
        windows/BGSoundsVolumeWindow.hpp

    PUBLIC
        include/application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp
)

target_link_libraries(application-bell-background-sounds
    PRIVATE
        app
        apps-common

    PUBLIC
        module-gui
)
