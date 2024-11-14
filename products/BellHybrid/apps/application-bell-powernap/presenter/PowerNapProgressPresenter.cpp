// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PowerNapProgressPresenter.hpp"
#include "data/PowerNapCommon.hpp"
#include "models/PowerNapFrontlightModel.hpp"

#include <common/models/TimeModel.hpp>
#include <db/SystemSettings.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <gsl/assert>
#include <Application.hpp>

namespace
{
    constexpr auto powerNapAlarmTimerName = "PowerNapAlarmTimer";
    constexpr std::chrono::seconds fadeInDuration{45};
} // namespace

namespace app::powernap
{
    PowerNapProgressPresenter::PowerNapProgressPresenter(app::ApplicationCommon *app,
                                                         settings::Settings *settings,
                                                         AbstractAudioModel &audioModel,
                                                         std::unique_ptr<AbstractTimeModel> timeModel,
                                                         std::unique_ptr<PowerNapFrontlightModel> frontlightModel,
                                                         const std::chrono::seconds &powerNapAlarmDuration)
        : app{app}, settings{settings}, audioModel{audioModel}, timeModel{std::move(timeModel)},
          frontlightModel{std::move(frontlightModel)},
          napAlarmTimer{sys::TimerFactory::createSingleShotTimer(
              app, powerNapAlarmTimerName, powerNapAlarmDuration, [this](sys::Timer &) { onNapAlarmFinished(); })}

    {}

    auto PowerNapProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) -> void
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() {
            getView()->progressFinished();
            onNapFinished();
        });
    }

    auto PowerNapProgressPresenter::activate() -> void
    {
        Expects(timer != nullptr);
        const auto &value = settings->getValue(powernapDBRecordName);
        static_cast<app::Application *>(app)->suspendIdleTimer();
        timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
        timer->start();
        napFinished = false;
    }

    auto PowerNapProgressPresenter::abortNap() -> void
    {
        napFinished = false;
        timer->stop();
        napAlarmTimer.stop();
    }

    auto PowerNapProgressPresenter::endNap() -> void
    {
        abortNap();
        onNapAlarmFinished();
    }

    auto PowerNapProgressPresenter::pause() -> void
    {
        timer->stop();
        getView()->pause();
    }

    auto PowerNapProgressPresenter::resume() -> void
    {
        timer->start();
        getView()->resume();
    }

    auto PowerNapProgressPresenter::onNapFinished() -> void
    {
        if (napFinished) {
            return;
        }

        frontlightModel->lockKeypressTrigger();
        if (frontlightModel->isAlarmLightEnabled()) {
            frontlightModel->startBrightnessFadeIn();
        }

        const auto &filePath    = settings->getValue(bell::settings::Alarm::tonePath, settings::SettingsScope::Global);
        const auto fadeInActive = utils::getNumericValue<bool>(settings->getValue(bell::settings::Alarm::fadeActive,
                                                                                  settings::SettingsScope::Global))
                                      ? audio::Fade::In
                                      : audio::Fade::Disable;

        audioModel.play(filePath,
                        AbstractAudioModel::PlaybackType::Alarm,
                        AbstractAudioModel::PlaybackMode::Single,
                        {},
                        audio::FadeParams{fadeInActive, fadeInDuration});
        napAlarmTimer.start();
        napFinished = true;
    }

    auto PowerNapProgressPresenter::onNapAlarmFinished() -> void
    {
        frontlightModel->unlockKeypressTrigger();
        frontlightModel->turnOff();
        audioModel.stopPlayedByThis({});
        getView()->napEnded();
    }

    auto PowerNapProgressPresenter::handleUpdateTimeEvent() -> void
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    auto PowerNapProgressPresenter::isNapFinished() -> bool
    {
        return napFinished;
    }

    auto PowerNapProgressPresenter::onBeforeShow() -> void
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    auto PowerNapProgressPresenter::isTimerStopped() -> bool
    {
        return timer->isStopped();
    }
} // namespace app::powernap
