// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapProgressPresenter.hpp"
#include "application-bell-powernap/ApplicationBellPowerNap.hpp"
#include "data/PowerNapCommon.hpp"

#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <common/models/TimeModel.hpp>
#include <db/SystemSettings.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>
#include <common/windows/SessionPausedWindow.hpp>

#include <gsl/assert>

namespace
{
    inline constexpr auto powernapAlarmTimerName = "PowerNapAlarmTimer";
    inline constexpr std::chrono::minutes powernapAlarmTimeout{5};
} // namespace

namespace app::powernap
{
    PowerNapProgressPresenter::PowerNapProgressPresenter(
        app::ApplicationCommon *app,
        settings::Settings *settings,
        std::unique_ptr<AbstractSoundsRepository> soundsRepository,
        AbstractAudioModel &audioModel,
        app::bell_settings::AbstractFrontlightModel &frontLightModel,
        std::unique_ptr<AbstractTimeModel> timeModel,
        std::unique_ptr<app::bell_settings::AlarmLightOnOffModel> alarmLightOnOffModel)
        : app{app}, settings{settings}, soundsRepository{std::move(soundsRepository)}, audioModel{audioModel},
          frontLightModel{frontLightModel}, timeModel{std::move(timeModel)}, alarmLightOnOffModel{std::move(
                                                                                 alarmLightOnOffModel)},
          napAlarmTimer{sys::TimerFactory::createSingleShotTimer(
              app, powernapAlarmTimerName, powernapAlarmTimeout, [this](sys::Timer &) { onNapAlarmFinished(); })}

    {}

    void PowerNapProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onNapFinished(); });
    }

    void PowerNapProgressPresenter::activate()
    {
        Expects(timer != nullptr);
        const auto value = settings->getValue(powernapDBRecordName);
        reinterpret_cast<app::Application *>(app)->suspendIdleTimer();
        timer->reset(std::chrono::minutes{utils::getNumericValue<int>(value)});
        timer->start();
        napFinished = false;
    }

    void PowerNapProgressPresenter::endNap()
    {
        napFinished = false;
        timer->stop();
        napAlarmTimer.stop();
        onNapAlarmFinished();
    }

    void PowerNapProgressPresenter::pause()
    {
        timer->stop();
        app->switchWindow(gui::window::session_paused::sessionPaused);
    }

    void PowerNapProgressPresenter::resume()
    {
        if (napFinished) {
            return;
        }
        timer->start();
    }

    void PowerNapProgressPresenter::onNapFinished()
    {
        if (napFinished) {
            return;
        }
        if (alarmLightOnOffModel->getValue()) {
            const auto modeAutoStr = utils::translate("app_bell_settings_frontlight_mode_auto");
            frontLightModel.setMode(frontLightModel.getModeModel().getValue() == modeAutoStr
                                        ? screen_light_control::ScreenLightMode::Automatic
                                        : screen_light_control::ScreenLightMode::Manual);
            frontLightModel.setBrightness(frontLightModel.getBrightnessModel().getValue());
            frontLightModel.setStatus(true);
        }

        const auto filePath = soundsRepository->titleToPath(
            settings->getValue(bell::settings::Alarm::tone, settings::SettingsScope::Global));

        audioModel.play(filePath.value_or(""), AbstractAudioModel::PlaybackType::Alarm, {});
        napAlarmTimer.start();
        napFinished = true;
    }

    void PowerNapProgressPresenter::onNapAlarmFinished()
    {
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

} // namespace app::powernap
