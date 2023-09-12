// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsPresenter.hpp"

#include <Timers/TimerFactory.hpp>
#include <algorithm>

namespace
{
    constexpr auto songProgressTimerInterval = std::chrono::seconds{1};
    constexpr auto songProgressTimerName     = "MusicPlayerSongProgress";
} // namespace

namespace app::music_player
{
    SongsPresenter::SongsPresenter(app::ApplicationCommon *app,
                                   std::shared_ptr<app::music::SongsModelInterface> songsModelInterface,
                                   std::unique_ptr<AbstractAudioOperations> &&audioOperations)
        : songsModelInterface{std::move(songsModelInterface)}, audioOperations{std::move(audioOperations)}
    {
        songProgressTimer = sys::TimerFactory::createPeriodicTimer(
            app, songProgressTimerName, songProgressTimerInterval, [this](sys::Timer &) { handleTrackProgressTick(); });
    }

    std::shared_ptr<app::music::SongsModelInterface> SongsPresenter::getMusicPlayerModelInterface() const
    {
        return songsModelInterface;
    }

    void SongsPresenter::createData()
    {
        songsModelInterface->createData([this](const std::string &fileName) { return requestAudioOperation(fileName); },
                                        [this]() { stop(); },
                                        [this](const UTF8 &text) { setViewNavBarTemporaryMode(text); },
                                        [this]() { restoreViewNavBarFromTemporaryMode(); });
        updateViewSongState();
    }

    bool SongsPresenter::play(const std::string &filePath)
    {
        waitingToPlay = true;
        return audioOperations->play(filePath, [this, filePath](audio::RetCode retCode, audio::Token token) {
            waitingToPlay = false;
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Playback audio operation failed, retcode = %s, token validity = %d",
                          str(retCode).c_str(),
                          token.IsValid());
                refreshView();
                const auto &nextSongToPlay = songsModelInterface->getNextFilePath(filePath);
                if (nextSongToPlay.empty()) {
                    return;
                }
                play(nextSongToPlay);
                return;
            }

            const app::music::SongContext songContext{
                app::music::SongState::Playing, token, filePath, app::music::SongContext::StartPos};
            songsModelInterface->setCurrentSongContext(songContext);

            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(app::music::SongState::Playing);
            }
            updateViewSongState();
            songsModelInterface->updateRepository(filePath);
            resetTrackProgressRatio();
            songProgressTimer.start();
            updateViewProgressState();
            refreshView();
        });
    }

    bool SongsPresenter::pause()
    {
        const auto &currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken.has_value()) {
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
                songsModelInterface->setCurrentSongState(app::music::SongState::NotPlaying);
                if (changePlayingStateCallback != nullptr) {
                    changePlayingStateCallback(app::music::SongState::NotPlaying);
                }
                updateViewSongState();
                songProgressTimer.stop();
                updateTrackProgressRatio();
                updateViewProgressState();
                refreshView();
            });
        }
        return false;
    }

    bool SongsPresenter::resume()
    {
        const auto &currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken.has_value()) {
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
                    songsModelInterface->setCurrentSongState(app::music::SongState::Playing);
                    if (changePlayingStateCallback != nullptr) {
                        changePlayingStateCallback(app::music::SongState::Playing);
                    }
                    updateViewSongState();
                    songProgressTimestamp = std::chrono::system_clock::now();
                    songProgressTimer.start();
                    refreshView();
                });
        }
        return false;
    }

    bool SongsPresenter::stop()
    {
        const auto &currentFileToken = songsModelInterface->getCurrentFileToken();
        if (currentFileToken.has_value()) {
            return audioOperations->stop(currentFileToken.value(), [this](audio::RetCode, audio::Token) {
                // The answer will come via multicast and will be handled in the application
                updateViewSongState();
                songProgressTimer.stop();
                resetTrackProgressRatio();
                updateViewProgressState();
                refreshView();
            });
        }
        return false;
    }

    bool SongsPresenter::playNext()
    {
        const auto &currentSongContext = songsModelInterface->getCurrentSongContext();
        const auto &nextSongToPlay     = songsModelInterface->getNextFilePath(currentSongContext.filePath);
        if (nextSongToPlay.empty()) {
            return false;
        }
        return play(nextSongToPlay);
    }

    bool SongsPresenter::playPrevious()
    {
        const auto &currentSongContext = songsModelInterface->getCurrentSongContext();
        const auto &prevSongToPlay     = songsModelInterface->getPreviousFilePath(currentSongContext.filePath);
        if (prevSongToPlay.empty()) {
            return false;
        }

        return play(prevSongToPlay);
    }

    void SongsPresenter::songsStateRequest()
    {
        updateViewSongState();
    }

    void SongsPresenter::progressStateRequest()
    {
        updateViewProgressState();
    }

    void SongsPresenter::setPlayingStateCallback(std::function<void(app::music::SongState)> cb)
    {
        changePlayingStateCallback = std::move(cb);
    }

    bool SongsPresenter::handleAudioStopNotifiaction(audio::Token token)
    {
        if (token == songsModelInterface->getCurrentFileToken()) {
            songsModelInterface->clearCurrentSongContext();
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(app::music::SongState::NotPlaying);
            }
            if (!waitingToPlay) {
                updateViewSongState();
                refreshView();
            }
            return true;
        }
        return false;
    }

    bool SongsPresenter::handleAudioEofNotification(audio::Token token)
    {
        const auto &currentSongContext = songsModelInterface->getCurrentSongContext();

        if (token == currentSongContext.currentFileToken) {
            songsModelInterface->clearCurrentSongContext();
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(app::music::SongState::NotPlaying);
            }
            auto nextSongToPlay = songsModelInterface->getNextFilePath(currentSongContext.filePath);
            if (!nextSongToPlay.empty()) {
                requestAudioOperation(nextSongToPlay);
            }
            else {
                updateViewSongState();
                refreshView();
            }
            return true;
        }
        return false;
    }

    bool SongsPresenter::handleAudioPausedNotification(audio::Token token)
    {
        if (token == songsModelInterface->getCurrentFileToken()) {
            songsModelInterface->setCurrentSongState(app::music::SongState::NotPlaying);
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(app::music::SongState::NotPlaying);
            }
            updateViewSongState();
            songProgressTimer.stop();
            updateTrackProgressRatio();
            updateViewProgressState();
            refreshView();
            return true;
        }
        return false;
    }

    bool SongsPresenter::handleAudioResumedNotification(audio::Token token)
    {
        if (token == songsModelInterface->getCurrentFileToken()) {
            songsModelInterface->setCurrentSongState(app::music::SongState::Playing);
            if (changePlayingStateCallback != nullptr) {
                changePlayingStateCallback(app::music::SongState::Playing);
            }
            updateViewSongState();
            songProgressTimestamp = std::chrono::system_clock::now();
            songProgressTimer.start();
            refreshView();
            return true;
        }
        return false;
    }

    bool SongsPresenter::handlePlayOrPauseRequest()
    {
        const auto &currentSongContext = songsModelInterface->getCurrentSongContext();
        if (currentSongContext.isPaused()) {
            return resume();
        }
        else if (currentSongContext.isPlaying()) {
            return pause();
        }
        return false;
    }

    void SongsPresenter::updateTrackProgressRatio()
    {
        std::uint32_t secondsTotal = 0;
        if (songsModelInterface->getActivatedRecord().has_value()) {
            secondsTotal = songsModelInterface->getActivatedRecord()->audioProperties.songLength;
        }

        const auto now = std::chrono::system_clock::now();
        const auto millisecondsToAdd =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - songProgressTimestamp);
        songMillisecondsElapsed += millisecondsToAdd;
        songProgressTimestamp = now;

        if (secondsTotal == 0) {
            currentProgressRatio = 0.0f;
        }
        else {
            currentProgressRatio =
                static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(songMillisecondsElapsed).count()) /
                static_cast<float>(secondsTotal);
        }
        currentProgressRatio = std::clamp(currentProgressRatio, 0.0f, 1.0f);
    }

    void SongsPresenter::resetTrackProgressRatio()
    {
        currentProgressRatio    = 0.0f;
        songMillisecondsElapsed = std::chrono::milliseconds::zero();
        songProgressTimestamp   = std::chrono::system_clock::now();
        updateTrackProgressRatio();
    }

    void SongsPresenter::handleTrackProgressTick()
    {
        updateTrackProgressRatio();
        updateViewProgressState();
        refreshView();
    }

    bool SongsPresenter::requestAudioOperation(const std::string &filePath)
    {
        auto currentSongContext = songsModelInterface->getCurrentSongContext();
        if (currentSongContext.isValid() && (filePath.empty() || currentSongContext.filePath == filePath)) {
            return currentSongContext.isPlaying() ? pause() : resume();
        }
        return play(filePath);
    }

    void SongsPresenter::setViewNavBarTemporaryMode(const std::string &text)
    {
        if (auto view = getView(); view != nullptr) {
            view->setNavBarTemporaryMode(text);
        }
    }

    void SongsPresenter::restoreViewNavBarFromTemporaryMode()
    {
        if (auto view = getView(); view != nullptr) {
            view->restoreFromNavBarTemporaryMode();
        }
    }

    void SongsPresenter::updateViewSongState()
    {
        if (auto view = getView(); view != nullptr) {
            auto currentSongContext = songsModelInterface->getCurrentSongContext();
            auto currentRecord      = songsModelInterface->getActivatedRecord();
            view->updateSongsState(currentRecord,
                                   currentSongContext.isPlaying()
                                       ? SongsContract::View::RecordState::Playing
                                       : (currentSongContext.isPaused() ? SongsContract::View::RecordState::Paused
                                                                        : SongsContract::View::RecordState::Stopped));
        }
    }

    void SongsPresenter::updateViewProgressState()
    {
        if (auto view = getView(); view != nullptr) {
            view->updateSongProgress(currentProgressRatio);
        }
    }

    void SongsPresenter::refreshView()
    {
        if (auto view = getView(); view != nullptr) {
            view->refreshWindow();
        }
    }
} // namespace app::music_player
