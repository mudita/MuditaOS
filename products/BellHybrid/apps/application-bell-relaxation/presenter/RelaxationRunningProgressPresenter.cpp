// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningProgressPresenter.hpp"
#include "data/RelaxationCommon.hpp"
#include "widgets/RelaxationPlayer.hpp"

#include <common/models/TimeModel.hpp>
#include <gsl/assert>

#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <service-db/Settings.hpp>

namespace
{
    bool isSongLengthEqualToPeriod(std::chrono::seconds songLength, std::chrono::minutes period)
    {
        const auto periodInSeconds = std::chrono::duration_cast<std::chrono::seconds>(period);
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
        const auto settingsValue  = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        const auto presetDuration = utils::getNumericValue<int>(settingsValue);
        const auto songLength     = std::chrono::seconds{song.audioProperties.songLength};
        auto playbackDuration     = songLength;

        if (utils::is_number(settingsValue) && (presetDuration != 0) &&
            !isSongLengthEqualToPeriod(songLength, std::chrono::minutes{presetDuration})) {
            playbackDuration = std::chrono::minutes{presetDuration};
            timer->reset(playbackDuration);
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
            }
        };

        auto onFinishedCallback = [this](AbstractAudioModel::PlaybackFinishStatus status) {
            if (status == AbstractAudioModel::PlaybackFinishStatus::Error) {
                timer->stop();
                getView()->handleDeletedFile(); // Deleted file is currently the only error handled by player
            }
        };

        player.start(
            song.fileInfo.path, mode, std::move(onStartCallback), std::move(onFinishedCallback), playbackDuration);
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
        if (!timer->isStopped()) {
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
