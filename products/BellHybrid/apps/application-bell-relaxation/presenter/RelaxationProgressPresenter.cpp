// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationProgressPresenter.hpp"
#include "data/RelaxationCommon.hpp"
#include "widgets/RelaxationPlayer.hpp"
#include <ApplicationBellRelaxation.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <common/models/TimeModel.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <gsl/assert>

namespace app::relaxation
{
    RelaxationProgressPresenter::RelaxationProgressPresenter(settings::Settings *settings,
                                                             AbstractRelaxationPlayer &player,
                                                             std::unique_ptr<AbstractTimeModel> timeModel)
        : settings{settings}, player{player}, timeModel{std::move(timeModel)}
    {}

    void RelaxationProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onFinished(); });
    }

    void RelaxationProgressPresenter::activate(const db::multimedia_files::MultimediaFilesRecord &song)
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
    void RelaxationProgressPresenter::stop()
    {
        onFinished();
        timer->stop();
    }
    void RelaxationProgressPresenter::onFinished()
    {
        auto onStopCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                getView()->onFinished();
            }
        };
        player.stop(std::move(onStopCallback));
    }
    void RelaxationProgressPresenter::pause()
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
    void RelaxationProgressPresenter::resume()
    {
        if (timer->isStopped()) {
            auto onResumeCallback = [this](audio::RetCode retCode) {
                if (retCode == audio::RetCode::Success) {
                    timer->start();
                }
            };
            player.resume(std::move(onResumeCallback));
        }
    }
    void RelaxationProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }
    bool RelaxationProgressPresenter::isPaused()
    {
        return player.isPaused();
    }
    void RelaxationProgressPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }
} // namespace app::relaxation
