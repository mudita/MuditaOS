// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsPresenter.hpp"

namespace app::music_player
{
    using SongState = SongsModelInterface::SongState;

    SongsPresenter::SongsPresenter(std::shared_ptr<app::music_player::SongsModelInterface> songsModelInterface,
                                   std::unique_ptr<AbstractAudioOperations> &&audioOperations)
        : songsModelInterface{std::move(songsModelInterface)}, audioOperations{std::move(audioOperations)}
    {}

    std::shared_ptr<app::music_player::SongsListItemProvider> SongsPresenter::getMusicPlayerItemProvider() const
    {
        return songsModelInterface;
    }

    void SongsPresenter::createData(std::function<bool(const std::string &fileName)> fn)
    {
        songsModelInterface->createData(fn);
    }

    bool SongsPresenter::play(const std::string &filePath)
    {
        songsModelInterface->setCurrentSongState(SongState::Playing);
        return audioOperations->play(filePath,
                                     [this](audio::Token token) { songsModelInterface->setCurrentFileToken(token); });
    }

    bool SongsPresenter::pause()
    {
        auto currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken) {
            songsModelInterface->setCurrentSongState(SongState::NotPlaying);
            return audioOperations->pause(currentFileToken.value());
        }
        return false;
    }

    bool SongsPresenter::resume()
    {
        auto currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken) {
            songsModelInterface->setCurrentSongState(SongState::Playing);
            return audioOperations->resume(currentFileToken.value());
        }
        return false;
    }

    bool SongsPresenter::stop()
    {
        auto currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken) {
            songsModelInterface->setCurrentSongState(SongState::NotPlaying);
            return audioOperations->stop(currentFileToken.value(), [this](audio::Token token) {
                if (token == songsModelInterface->getCurrentFileToken()) {
                    songsModelInterface->setCurrentFileToken(std::nullopt);
                    songsModelInterface->setCurrentSongState(SongState::NotPlaying);
                }
            });
        }
        return false;
    }

    void SongsPresenter::togglePlaying()
    {
        if (songsModelInterface->isSongPlaying()) {
            pause();
        }
        else {
            resume();
        }
    }

} // namespace app::music_player
