// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningProgressPresenter.hpp"
#include "data/RelaxationCommon.hpp"
#include "widgets/RelaxationPlayer.hpp"

#include <common/models/TimeModel.hpp>
#include <gsl/assert>

namespace
{
    bool songLengthEqualsToSelectedPeriod(std::chrono::minutes period, std::chrono::seconds songLength)
    {
        auto periodInSeconds = std::chrono::duration_cast<std::chrono::seconds>(period);
        return periodInSeconds.count() == songLength.count();
    }
} // namespace

namespace app::relaxation
{
    RelaxationRunningProgressPresenter::RelaxationRunningProgressPresenter(settings::Settings *settings,
                                                                           AbstractRelaxationPlayer &player,
                                                                           std::unique_ptr<AbstractTimeModel> timeModel)
        : settings{settings}, player{player}, timeModel{std::move(timeModel)}
    {
        timerFinished = false;
    }

    void RelaxationRunningProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() {
            timerFinished = true;
            onFinished();
        });
    }

    void RelaxationRunningProgressPresenter::activate(const db::multimedia_files::MultimediaFilesRecord &song)
    {
        Expects(timer != nullptr);
        AbstractRelaxationPlayer::PlaybackMode mode;
        const auto value      = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        const auto songLength = std::chrono::seconds{song.audioProperties.songLength};
        if (utils::is_number(value) && utils::getNumericValue<int>(value) != 0 &&
            !songLengthEqualsToSelectedPeriod(std::chrono::minutes{utils::getNumericValue<int>(value)}, songLength)) {
            const auto playbackTimeInMinutes = std::chrono::minutes{utils::getNumericValue<int>(value)};
            timer->reset(playbackTimeInMinutes);
            mode = AbstractRelaxationPlayer::PlaybackMode::Looped;
        }
        else {
            mode = AbstractRelaxationPlayer::PlaybackMode::SingleShot;
            if (songLength > std::chrono::seconds::zero()) {
                timer->reset(songLength);
            }
            else {
                getView()->handleError();
                return;
            }
        }
        auto onStartCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                timer->start();
            }
            else {
                getView()->handleError();
                return;
            }
        };

        auto onErrorCallback = [this]() { getView()->handleDeletedFile(); };
        player.start(song.fileInfo.path, mode, std::move(onStartCallback), std::move(onErrorCallback));
    }

    void RelaxationRunningProgressPresenter::stop()
    {
        onFinished();
        timer->stop();
    }

    void RelaxationRunningProgressPresenter::onFinished()
    {
        auto onStopCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                getView()->onFinished();
            }
        };
        player.stop(std::move(onStopCallback));
    }

    void RelaxationRunningProgressPresenter::pause()
    {
        if (not timer->isStopped()) {
            auto onPauseCallback = [this](audio::RetCode retCode) {
                if (retCode == audio::RetCode::Success) {
                    timer->stop();
                    getView()->onPaused();
                }
            };
            player.pause(std::move(onPauseCallback));
        }
    }

    void RelaxationRunningProgressPresenter::resume()
    {
        if (timer->isStopped()) {
            auto onResumeCallback = [this](audio::RetCode retCode) {
                if (retCode == audio::RetCode::Success) {
                    timer->start();
                    getView()->resume();
                }
            };
            player.resume(std::move(onResumeCallback));
        }
    }

    void RelaxationRunningProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    bool RelaxationRunningProgressPresenter::isPaused()
    {
        return player.isPaused();
    }

    void RelaxationRunningProgressPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    bool RelaxationRunningProgressPresenter::isTimerStopped()
    {
        return timer->isStopped();
    }

    bool RelaxationRunningProgressPresenter::isTimerFinished()
    {
        return timerFinished;
    }
} // namespace app::relaxation
