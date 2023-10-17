// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningLoopPresenter.hpp"
#include "data/RelaxationCommon.hpp"
#include "widgets/RelaxationPlayer.hpp"

#include <common/models/TimeModel.hpp>
#include <gsl/assert>

namespace app::relaxation
{
    RelaxationRunningLoopPresenter::RelaxationRunningLoopPresenter(settings::Settings *settings,
                                                                   AbstractRelaxationPlayer &player,
                                                                   AbstractBatteryModel &battery,
                                                                   std::unique_ptr<AbstractTimeModel> timeModel)
        : settings{settings}, player{player}, batteryModel{battery}, timeModel{std::move(timeModel)}
    {}

    void RelaxationRunningLoopPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() {
            onFinished();
            getView()->onFinished();
        });
    }

    void RelaxationRunningLoopPresenter::activate(const db::multimedia_files::MultimediaFilesRecord &song)
    {
        Expects(timer != nullptr);
        AbstractRelaxationPlayer::PlaybackMode mode;
        const auto value = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        if (utils::is_number(value) && utils::getNumericValue<int>(value) != 0) {
            timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
            mode = AbstractRelaxationPlayer::PlaybackMode::Looped;
        }
        else {
            const auto songLength = std::chrono::seconds{song.audioProperties.songLength};
            mode                  = AbstractRelaxationPlayer::PlaybackMode::SingleShot;

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

    void RelaxationRunningLoopPresenter::stop()
    {
        onFinished();
        timer->stop();
    }

    void RelaxationRunningLoopPresenter::onFinished()
    {
        auto onStopCallback = [this](audio::RetCode retCode) {};
        player.stop(std::move(onStopCallback));
    }

    void RelaxationRunningLoopPresenter::pause()
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

    void RelaxationRunningLoopPresenter::resume()
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

    void RelaxationRunningLoopPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    bool RelaxationRunningLoopPresenter::isPaused()
    {
        return player.isPaused();
    }

    void RelaxationRunningLoopPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    bool RelaxationRunningLoopPresenter::isTimerStopped()
    {
        return timer->isStopped();
    }

    Store::Battery RelaxationRunningLoopPresenter::handleBatteryStatus()
    {
        return batteryModel.getLevelState();
    }
} // namespace app::relaxation
