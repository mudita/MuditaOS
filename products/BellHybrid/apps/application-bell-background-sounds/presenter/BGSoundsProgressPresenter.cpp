// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsProgressPresenter.hpp"
#include "data/BGSoundsCommon.hpp"
#include "widgets/BGSoundsPlayer.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <common/models/TimeModel.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <gsl/assert>

namespace app::bgSounds
{
    BGSoundsProgressPresenter::BGSoundsProgressPresenter(settings::Settings *settings,
                                                         AbstractBGSoundsPlayer &player,
                                                         std::unique_ptr<AbstractTimeModel> timeModel)
        : settings{settings}, player{player}, timeModel{std::move(timeModel)}
    {}

    void BGSoundsProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onFinished(); });
    }

    void BGSoundsProgressPresenter::activate(const db::multimedia_files::MultimediaFilesRecord &song)
    {
        Expects(timer != nullptr);
        AbstractBGSoundsPlayer::PlaybackMode mode;
        const auto value = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        if (utils::is_number(value) && utils::getNumericValue<int>(value) != 0) {
            timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
            mode = AbstractBGSoundsPlayer::PlaybackMode::Looped;
        }
        else {
            timer->reset(std::chrono::seconds{song.audioProperties.songLength});
            mode = AbstractBGSoundsPlayer::PlaybackMode::SingleShot;
        }
        auto onStartCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                timer->start();
            }
        };
        player.start(song.fileInfo.path, mode, std::move(onStartCallback));
    }
    void BGSoundsProgressPresenter::stop()
    {
        onFinished();
        timer->stop();
    }
    void BGSoundsProgressPresenter::onFinished()
    {
        auto onStopCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                getView()->onFinished();
            }
        };
        player.stop(std::move(onStopCallback));
    }
    void BGSoundsProgressPresenter::pause()
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
    void BGSoundsProgressPresenter::resume()
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
    void BGSoundsProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }
    bool BGSoundsProgressPresenter::isPaused()
    {
        return player.isPaused();
    }
    void BGSoundsProgressPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }
} // namespace app::bgSounds
