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
        AudioNotificationsHandler.cpp
        models/SongContext.cpp
        models/SongsModel.cpp
        models/SongsRepository.cpp
        presenters/SongsPresenter.cpp
        widgets/SongItem.cpp
        windows/MusicPlayerMainWindow.cpp
        windows/MusicPlayerAllSongsWindow.cpp
    PRIVATE
        AudioNotificationsHandler.hpp
        data/MusicPlayerStyle.hpp
        models/SongContext.hpp
        models/SongsModel.hpp
        models/SongsRepository.hpp
        models/SongsModelInterface.hpp
        presenters/SongsPresenter.hpp
        widgets/SongItem.hpp
        windows/MusicPlayerMainWindow.cpp
        windows/MusicPlayerAllSongsWindow.hpp
    PUBLIC
        include/application-music-player/ApplicationMusicPlayer.hpp
)

target_link_libraries(application-music-player
    PRIVATE
        app
        i18n
        log
        module-gui
        service-audio
        utils-time
    PUBLIC
        apps-common
        module-audio
        tagsfetcher
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
