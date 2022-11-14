// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningProgressPresenter.hpp"
#include "data/RelaxationCommon.hpp"
#include "widgets/RelaxationPlayer.hpp"

#include <common/models/TimeModel.hpp>
#include <gsl/assert>

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
        const auto value = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        if (utils::is_number(value) && utils::getNumericValue<int>(value) != 0) {
            timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
            mode = AbstractRelaxationPlayer::PlaybackMode::Looped;
        }
        else {
            timer->reset(std::chrono::seconds{song.audioProperties.songLength});
            mode = AbstractRelaxationPlayer::PlaybackMode::SingleShot;
        }
        auto onStartCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                timer->start();
            }
        };
        player.start(song.fileInfo.path, mode, std::move(onStartCallback));
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
