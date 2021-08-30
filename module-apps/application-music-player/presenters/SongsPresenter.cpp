// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsPresenter.hpp"

#include <service-audio/AudioMessage.hpp>

namespace app::music_player
{
    SongsPresenter::SongsPresenter(std::shared_ptr<app::music_player::SongsModelInterface> songsModelInterface,
                                   std::unique_ptr<AbstractAudioOperations> &&audioOperations)
        : songsModelInterface{std::move(songsModelInterface)}, audioOperations{std::move(audioOperations)}
    {}

    std::shared_ptr<app::music_player::SongsListItemProvider> SongsPresenter::getMusicPlayerItemProvider() const
    {
        return songsModelInterface;
    }

    void SongsPresenter::createData()
    {
        songsModelInterface->createData([this](const std::string &fileName) { return requestAudioOperation(fileName); },
                                        [this]() { stop(); },
                                        [this](const UTF8 &text) { setViewBottomBarTemporaryMode(text); },
                                        [this]() { restoreViewBottomBarFromTemporaryMode(); });
        updateViewSongState();
    }

    bool SongsPresenter::play(const std::string &filePath)
    {
        return audioOperations->play(filePath, [this, filePath](audio::RetCode retCode, audio::Token token) {
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Playback audio operation failed, retcode = %s, token validity = %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            SongContext songToken{SongState::Playing, token, filePath};
            songsModelInterface->setCurrentSongContext(songToken);
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(SongState::Playing);
            }
            updateViewSongState();
        });
    }

    bool SongsPresenter::pause()
    {
        auto currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken) {
            return audioOperations->pause(currentFileToken.value(), [this](audio::RetCode retCode, audio::Token token) {
                if (retCode != audio::RetCode::Success || !token.IsValid()) {
                    LOG_ERROR("Pause audio operation failed, retcode = %s, token validity = %d",
                              str(retCode).c_str(),
                              token.IsValid());
                    return;
                }
                if (token != songsModelInterface->getCurrentFileToken()) {
                    LOG_ERROR("Pause audio operation failed, wrong token");
                    return;
                }
                songsModelInterface->setCurrentSongState(SongState::NotPlaying);
                if (changePlayingStateCallback != nullptr) {
                    changePlayingStateCallback(SongState::NotPlaying);
                }
                updateViewSongState();
            });
        }
        return false;
    }

    bool SongsPresenter::resume()
    {
        auto currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken) {
            return audioOperations->resume(
                currentFileToken.value(), [this](audio::RetCode retCode, audio::Token token) {
                    if (retCode != audio::RetCode::Success || !token.IsValid()) {
                        LOG_ERROR("Resume audio operation failed, retcode = %s, token validity = %d",
                                  str(retCode).c_str(),
                                  token.IsValid());
                        return;
                    }
                    if (token != songsModelInterface->getCurrentFileToken()) {
                        LOG_ERROR("Resume audio operation failed, wrong token");
                        return;
                    }
                    songsModelInterface->setCurrentSongState(SongState::Playing);
                    if (changePlayingStateCallback != nullptr) {
                        changePlayingStateCallback(SongState::Playing);
                    }
                    updateViewSongState();
                });
        }
        return false;
    }

    bool SongsPresenter::stop()
    {
        auto currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken) {
            return audioOperations->stop(currentFileToken.value(), [](audio::RetCode, audio::Token) {
                // The answer will come via multicast and will be handled in the application
            });
        }
        return false;
    }

    void SongsPresenter::setPlayingStateCallback(std::function<void(SongState)> cb)
    {
        changePlayingStateCallback = std::move(cb);
    }

    bool SongsPresenter::handleAudioStopNotifiaction(audio::Token token)
    {
        if (token == songsModelInterface->getCurrentFileToken()) {
            songsModelInterface->clearCurrentSongContext();
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(SongState::NotPlaying);
            }
            updateViewSongState();
            refreshView();
            return true;
        }
        return false;
    }

    bool SongsPresenter::handleAudioEofNotification(audio::Token token)
    {
        auto currentSongContext = songsModelInterface->getCurrentSongContext();

        if (token == currentSongContext.currentFileToken) {
            songsModelInterface->clearCurrentSongContext();
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(SongState::NotPlaying);
            }
            updateViewSongState();
            refreshView();
            auto nextSongToPlay = songsModelInterface->getNextFilePath(currentSongContext.filePath);
            if (!nextSongToPlay.empty()) {
                requestAudioOperation(nextSongToPlay);
            }
            return true;
        }
        return false;
    }

    bool SongsPresenter::handleAudioPausedNotification(audio::Token token)
    {
        if (token == songsModelInterface->getCurrentFileToken()) {
            songsModelInterface->setCurrentSongState(SongState::NotPlaying);
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(SongState::NotPlaying);
            }
            updateViewSongState();
            return true;
        }
        return false;
    }

    bool SongsPresenter::handleAudioResumedNotification(audio::Token token)
    {
        if (token == songsModelInterface->getCurrentFileToken()) {
            songsModelInterface->setCurrentSongState(SongState::Playing);
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(SongState::Playing);
            }
            updateViewSongState();
            return true;
        }
        return false;
    }

    bool SongsPresenter::requestAudioOperation(const std::string &filePath)
    {
        auto currentSongContext = songsModelInterface->getCurrentSongContext();
        if (currentSongContext.isValid() && (filePath.empty() || currentSongContext.filePath == filePath)) {
            return currentSongContext.isPlaying() ? pause() : resume();
        }
        return play(filePath);
    }

    void SongsPresenter::setViewBottomBarTemporaryMode(const std::string &text)
    {
        if (auto view = getView(); view != nullptr) {
            view->setBottomBarTemporaryMode(text);
        }
    }

    void SongsPresenter::restoreViewBottomBarFromTemporaryMode()
    {
        if (auto view = getView(); view != nullptr) {
            view->restoreFromBottomBarTemporaryMode();
        }
    }

    void SongsPresenter::updateViewSongState()
    {
        if (auto view = getView(); view != nullptr) {
            view->updateSongsState();
        }
    }

    void SongsPresenter::refreshView()
    {
        if (auto view = getView(); view != nullptr) {
            view->refreshWindow();
        }
    }
} // namespace app::music_player
