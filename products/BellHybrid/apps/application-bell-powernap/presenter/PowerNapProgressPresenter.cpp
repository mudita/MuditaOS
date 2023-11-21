// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
} // namespace

namespace app::powernap
{
    PowerNapProgressPresenter::PowerNapProgressPresenter(app::ApplicationCommon *app,
                                                         settings::Settings *settings,
                                                         std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                                                         AbstractAudioModel &audioModel,
                                                         std::unique_ptr<AbstractTimeModel> timeModel,
                                                         std::unique_ptr<PowerNapFrontlightModel> frontlightModel,
                                                         const std::chrono::seconds &powerNapAlarmDuration)
        : app{app}, settings{settings}, soundsRepository{std::move(soundsRepository)},
          audioModel{audioModel}, timeModel{std::move(timeModel)}, frontlightModel{std::move(frontlightModel)},
          napAlarmTimer{sys::TimerFactory::createSingleShotTimer(
              app, powerNapAlarmTimerName, powerNapAlarmDuration, [this](sys::Timer &) { onNapAlarmFinished(); })}

    {}

    void PowerNapProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() {
            getView()->progressFinished();
            onNapFinished();
        });
    }

    void PowerNapProgressPresenter::activate()
    {
        Expects(timer != nullptr);
        const auto value = settings->getValue(powernapDBRecordName);
        static_cast<app::Application *>(app)->suspendIdleTimer();
        timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
        timer->start();
        napFinished = false;
    }

    void PowerNapProgressPresenter::abortNap()
    {
        napFinished = false;
        timer->stop();
        napAlarmTimer.stop();
    }

    void PowerNapProgressPresenter::endNap()
    {
        abortNap();
        onNapAlarmFinished();
    }

    void PowerNapProgressPresenter::pause()
    {
        timer->stop();
        getView()->pause();
    }

    void PowerNapProgressPresenter::resume()
    {
        timer->start();
        getView()->resume();
    }

    void PowerNapProgressPresenter::onNapFinished()
    {
        if (napFinished) {
            return;
        }

        if (frontlightModel->isAlarmLightEnabled()) {
            frontlightModel->lockKeypressTrigger();
            frontlightModel->startBrightnessFadeIn();
        }

        const auto &filePath = soundsRepository->titleToPath(
            settings->getValue(bell::settings::Alarm::tone, settings::SettingsScope::Global));

        audioModel.play(filePath.value_or(""), AbstractAudioModel::PlaybackType::Alarm, {});
        napAlarmTimer.start();
        napFinished = true;
    }

    void PowerNapProgressPresenter::onNapAlarmFinished()
    {
        frontlightModel->unlockKeypressTrigger();
        frontlightModel->turnOff();
        audioModel.stopPlayedByThis({});
        getView()->napEnded();
    }

    void PowerNapProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    bool PowerNapProgressPresenter::isNapFinished()
    {
        return napFinished;
    }

    void PowerNapProgressPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    bool PowerNapProgressPresenter::isTimerStopped()
    {
        return timer->isStopped();
    }
} // namespace app::powernap
