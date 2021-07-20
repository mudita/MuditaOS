# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-music-player STATIC)

target_include_directories(application-music-player
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-music-player
    PRIVATE
        ApplicationMusicPlayer.cpp
        models/SongsModel.cpp
        models/SongsRepository.cpp
        presenters/AudioOperations.cpp
        presenters/SongsPresenter.cpp
        widgets/Action.cpp
        widgets/SongItem.cpp
        windows/MusicPlayerAllSongsWindow.cpp
        windows/MusicPlayerEmptyWindow.cpp
    PRIVATE
        data/MusicPlayerStyle.hpp
        models/SongsModel.hpp
        models/SongsRepository.hpp
        models/SongsModelInterface.hpp
        presenters/AudioOperations.hpp
        presenters/SongsPresenter.hpp
        widgets/Action.hpp
        widgets/SongItem.hpp
        windows/MusicPlayerAllSongsWindow.hpp
        windows/MusicPlayerEmptyWindow.hpp
    PUBLIC
        include/application-music-player/ApplicationMusicPlayer.hpp
)

option(ENABLE_APP_MUSIC_PLAYER "Enable application music player" ON)

target_compile_definitions(application-music-player
    INTERFACE
        $<$<BOOL:${ENABLE_APP_MUSIC_PLAYER}>:ENABLE_APP_MUSIC_PLAYER>
)

target_link_libraries(application-music-player
    PRIVATE
        i18n
        log
        module-gui
        service-audio
        time
    PUBLIC
        apps-common
        module-audio
)
